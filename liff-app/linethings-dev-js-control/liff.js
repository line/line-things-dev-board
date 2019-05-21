const deviceUUIDSet = new Set();
const connectedUUIDSet = new Set();
const connectingUUIDSet = new Set();
const notificationUUIDSet = new Set();

let logNumber = 1;

function onScreenLog(text) {
  const logbox = document.getElementById('logbox');
  logbox.value += '#' + logNumber + '> ';
  logbox.value += text;
  logbox.value += '\n';
  logbox.scrollTop = logbox.scrollHeight;
  logNumber++;
}

window.onload = () => {
  liff.init(async () => {
    onScreenLog('LIFF initialized');
    renderVersionField();

    await liff.initPlugins(['bluetooth']);
    onScreenLog('BLE plugin initialized');

    checkAvailablityAndDo(() => {
      onScreenLog('Finding devices...');
      findDevice();
    });
  }, e => {
    flashSDKError(e);
    onScreenLog(`ERROR on getAvailability: ${e}`);
  });
}

async function checkAvailablityAndDo(callbackIfAvailable) {
  const isAvailable = await liff.bluetooth.getAvailability().catch(e => {
    flashSDKError(e);
    onScreenLog(`ERROR on getAvailability: ${e}`);
    return false;
  });

  if (isAvailable) {
    document.getElementById('alert-liffble-notavailable').style.display = 'none';
    callbackIfAvailable();
  } else {
    document.getElementById('alert-liffble-notavailable').style.display = 'block';
    setTimeout(() => checkAvailablityAndDo(callbackIfAvailable), 1000);
  }
}

// Find LINE Things device using requestDevice()
async function findDevice() {
  const device = await liff.bluetooth.requestDevice().catch(e => {
    flashSDKError(e);
    onScreenLog(`ERROR on requestDevice: ${e}`);
    throw e;
  });

  try {
    if (!deviceUUIDSet.has(device.id)) {
      deviceUUIDSet.add(device.id);
      addDeviceToList(device);
    } else {
      // TODO: Maybe this is unofficial hack > device.rssi
      document.querySelector(`#${device.id} .rssi`).innerText = device.rssi;
    }

    checkAvailablityAndDo(() => setTimeout(findDevice, 100));
  } catch (e) {
    onScreenLog(`ERROR on findDevice: ${e}\n${e.stack}`);
  }
}

// Add device to found device list
function addDeviceToList(device) {
  onScreenLog('Device found: ' + device.name);

  const deviceList = document.getElementById('device-list');
  const deviceItem = document.getElementById('device-list-item').cloneNode(true);
  deviceItem.setAttribute('id', device.id);
  deviceItem.querySelector(".device-id").innerText = device.id;
  deviceItem.querySelector(".device-name").innerText = device.name;
  deviceItem.querySelector(".rssi").innerText = device.rssi;
  deviceItem.classList.add("d-flex");
  deviceItem.addEventListener('click', () => {
    deviceItem.classList.add("active");
    connectDevice(device);
  });
  deviceList.appendChild(deviceItem);
}

// Select target device and connect it
async function connectDevice(device) {
  onScreenLog('Device selected: ' + device.name);

  if (!device) {
    onScreenLog('No devices found. You must request a device first.');
  } else if (connectingUUIDSet.has(device.id) || connectedUUIDSet.has(device.id)) {
    onScreenLog('Already connected to this device.');
  } else {
    const board = new ThingsDevBoard(device);
    connectingUUIDSet.add(device.id);
    
    try {
      initializeCardForDevice(board);

      // Wait until the requestDevice call finishes before setting up the disconnect listner
      const disconnectCallback = () => {
        updateConnectionStatus(device, 'disconnected');
        device.removeEventListener('gattserverdisconnected', disconnectCallback);
      };
      device.addEventListener('gattserverdisconnected', disconnectCallback);

      onScreenLog('Connecting ' + device.name);
      await board.connect();

      // Check dev board firmware version
      await versionCheck(board);

      updateConnectionStatus(device, 'connected');
    } catch (e) {
      updateConnectionStatus(device, 'error');
      onScreenLog(`Connecting device failed: ${e}\n${e.stack}`);
    } finally {
      connectingUUIDSet.delete(device.id);
    }
  }
}

// Setup device information card
function initializeCardForDevice(board) {
  const device = board.device;
  const template = document.getElementById('device-template').cloneNode(true);
  const cardId = 'device-' + device.id;

  template.style.display = 'block';
  template.setAttribute('id', cardId);
  template.querySelector('.card > .card-header > .device-name').innerText = device.name;

  // Device disconnect button
  template.querySelector('.device-disconnect').addEventListener('click', () => {
    onScreenLog('Clicked disconnect button');
    device.gatt.disconnect();
  });

  template.querySelector('.device-read').addEventListener('click', async () => {
    const values = await board.deviceRead().catch(e => onScreenLog(`ERROR on deviceRead(): ${e}\n${e.stack}`));
    if (values) {
      getDeviceReadData(device).innerText = buf2hex(values.buffer);
    }
  });

  template.querySelector('.textctrl-write').addEventListener('click', () => {
    board.displayControl(
      parseInt(template.querySelector('.displayaddress_x').value, 16),
      parseInt(template.querySelector('.displayaddress_y').value, 16)
    ).catch(e => onScreenLog(`ERROR on displayControl(): ${e}\n${e.stack}`));
  });

  template.querySelector('.textsize-write').addEventListener('click', () => {
    board.displayFontSize(
      parseInt(template.querySelector('.textsize').value, 16)
    ).catch(e => onScreenLog(`ERROR on displayFontSize(): ${e}\n${e.stack}`));
  });

  template.querySelector('.text-write').addEventListener('click', () => {
    board.displayWrite(
      template.querySelector('.display_text').value
    ).catch(e => onScreenLog(`ERROR on displayWrite(): ${e}\n${e.stack}`));
  });

  template.querySelector('.text-clear').addEventListener('click', () => {
    board.displayClear().catch(e => onScreenLog(`ERROR on displayClear(): ${e}\n${e.stack}`));
  });

  template.querySelector('.led-write').addEventListener('click', () => {
    board.ledWrite(
      parseInt(template.querySelector('.led-port').value, 16),
      parseInt(template.querySelector('.led-value').value, 16)
    ).catch(e => onScreenLog(`ERROR on writeLed(): ${e}\n${e.stack}`));
  });

  template.querySelector('.buzzer-write').addEventListener('click', () => {
    board.buzzerControl(
      parseInt(template.querySelector('.buzzer-control').value, 16)
    ).catch(e => onScreenLog(`ERROR on buzzerControl(): ${e}\n${e.stack}`));
  });

  template.querySelector('.gpio-direction').addEventListener('click', () => {
    board.gpioPinMode(
      parseInt(template.querySelector('.gpio-direction-port').value, 16),
      parseInt(template.querySelector('.gpio-direction-dir').value, 16)
    ).catch(e => onScreenLog(`ERROR on gpioPinMode(): ${e}\n${e.stack}`));
  });

  template.querySelector('.gpio-dwrite').addEventListener('click', () => {
    board.gpioDigitalWrite(
      parseInt(template.querySelector('.gpio-digitalwrite-port').value, 16),
      parseInt(template.querySelector('.gpio-digitalwrite-value').value, 16)
    ).catch(e => onScreenLog(`ERROR on gpioDirigtalWrite(): ${e}\n${e.stack}`));
  });

  template.querySelector('.gpio-awrite').addEventListener('click', () => {
    board.gpioAnalogWrite(
      parseInt(template.querySelector('.gpio-awrite-port').value, 16),
      parseInt(template.querySelector('.gpio-awrite-value').value, 16)
    ).catch(e => onScreenLog(`ERROR on gpioAnalogWrite(): ${e}\n${e.stack}`));
  });

  template.querySelector('.i2c-start').addEventListener('click', () => {
    board.i2cStartTransaction(
      parseInt(template.querySelector('.i2c-start-addr').value, 16),
    ).catch(e => onScreenLog(`ERROR on i2cStartTransaction(): ${e}\n${e.stack}`));
  });

  template.querySelector('.i2c-write').addEventListener('click', () => {
    board.i2cWrite(
      parseInt(template.querySelector('.i2c-write-data').value, 16),
    ).catch(e => onScreenLog(`ERROR on i2cWrite(): ${e}\n${e.stack}`));
  });

  template.querySelector('.i2c-stop').addEventListener('click', () => {
    board.i2cStopTransaction()
      .catch(e => onScreenLog(`ERROR on i2cStopTransaction(): ${e}\n${e.stack}`));
  });

  template.querySelector('.i2c-request').addEventListener('click', () => {
    board.i2cRequestFrom(
      parseInt(template.querySelector('.i2c-request-addr').value, 16),
    ).catch(e => onScreenLog(`ERROR on i2cRequestFrom(): ${e}\n${e.stack}`));
  });

  template.querySelector('.i2c-readreq').addEventListener('click', () => {
    board.i2cReadRequest()
      .catch(e => onScreenLog(`ERROR on i2cReadRequest(): ${e}\n${e.stack}`));
  });

  template.querySelector('.gpio-dreadreq').addEventListener('click', () => {
    board.gpioDigitalReadReq(
      parseInt(template.querySelector('.gpio-dreadreq-port').value, 16),
    ).catch(e => onScreenLog(`ERROR on gpioDigitalReadReq(): ${e}\n${e.stack}`));
  });

  template.querySelector('.gpio-areadreq').addEventListener('click', () => {
    board.gpioAnalogReadReq(
      parseInt(template.querySelector('.gpio-areadreq-port').value, 16),
    ).catch(e => onScreenLog(`ERROR on gpioAnalogReadReq(): ${e}\n${e.stack}`));
  });

  template.querySelector('.read-buffer-write').addEventListener('click', () => {
    board.readReq(
      parseInt(template.querySelector('.read-buffer-source').value, 16),
    ).catch(e => onScreenLog(`ERROR on readReq(): ${e}\n${e.stack}`));
  });

  template.querySelector('.setuuid').addEventListener('click', () => {
    onScreenLog(`Write new Service UUID`);
    board.writeAdvertUuid(
      template.querySelector('.uuid_text').value
    ).catch(e => onScreenLog(`ERROR on writeAdvertuuid(): ${e}\n${e.stack}`));
  });

  template.querySelector('.notify-sw-enable').addEventListener('click', () => {
    board.swNotifyEnable(
      parseInt(template.querySelector('.notify-sw-source').value, 16), //Select Switch
      parseInt(template.querySelector('.notify-sw-mode').value, 16), //mode
      parseInt(template.querySelector('.notify-sw-interval').value, 16), //Interval
      notificationSwCallback //callback
    ).catch(e => onScreenLog(`SW Notify set error`));
  });


  template.querySelector('.notify-temp-enable').addEventListener('click', () => {
    board.tempNotifyEnable(
      parseInt(template.querySelector('.notify-temp-interval').value, 16), //Interval
      notificationTempCallback //callback
    ).catch(e => onScreenLog(`Temperature Notify set error`));
  });

  template.querySelector('.notify-sw-disable').addEventListener('click', () => {
    board.swNotifyDisable().catch(e => onScreenLog(`SW Notify disable error`));
  });

  template.querySelector('.notify-temp-disable').addEventListener('click', () => {
    board.tempNotifyDisable().catch(e => onScreenLog(`Temperature Notify disable error`));
  });

  // Tabs
  ['write', 'read', 'notify', 'advert'].map(key => {
    const tab = template.querySelector(`#nav-${key}-tab`);
    const nav = template.querySelector(`#nav-${key}`);

    tab.id = `nav-${key}-tab-${device.id}`;
    nav.id = `nav-${key}-${device.id}`;

    tab.href = '#' + nav.id;
    tab['aria-controls'] = nav.id;
    nav['aria-labelledby'] = tab.id;
  })

  // Remove existing same id card
  const oldCardElement = getDeviceCard(device);
  if (oldCardElement && oldCardElement.parentNode) {
    oldCardElement.parentNode.removeChild(oldCardElement);
  }

  document.getElementById('device-cards').appendChild(template);
  onScreenLog('Device card initialized: ' + device.name);
}

function notificationSwCallback(e) {
  const dataBuffer = new DataView(e.target.value.buffer);
  onScreenLog(`Notify SW ${e.target.uuid}: ${buf2hex(e.target.value.buffer)}`);
}

function notificationTempCallback(e) {
  const dataBuffer = new DataView(e.target.value.buffer);
  onScreenLog(`Notify Temperature : ` + String(((dataBuffer.getInt8(0) << 8) + dataBuffer.getInt8(1)) / 100));
}

//Version Check
async function versionCheck(board) {
  const version = await board.deviceVersionRead();
  if (version > 1) {
    onScreenLog('Firmware Version : ' + version);
  } else {
    onScreenLog('Do not support this mode. Please update device firmware. Version : ' + version);
    window.alert('Do not support this mode. Please update device firmware');
  }
}

// Update Connection Status
function updateConnectionStatus(device, status) {
  if (status == 'connected') {
    onScreenLog('Connected to ' + device.name);
    connectedUUIDSet.add(device.id);

    const statusBtn = getDeviceStatusButton(device);
    statusBtn.setAttribute('class', 'device-status btn btn-outline-primary btn-sm disabled');
    statusBtn.innerText = "Connected";
    getDeviceDisconnectButton(device).style.display = 'inline-block';
    getDeviceCardBody(device).style.display = 'block';
  } else if (status == 'disconnected') {
    onScreenLog('Disconnected from ' + device.name);
    connectedUUIDSet.delete(device.id);

    const statusBtn = getDeviceStatusButton(device);
    statusBtn.setAttribute('class', 'device-status btn btn-outline-secondary btn-sm disabled');
    statusBtn.innerText = "Disconnected";
    getDeviceDisconnectButton(device).style.display = 'none';
    getDeviceCardBody(device).style.display = 'none';
    document.getElementById(device.id).classList.remove('active');
  } else {
    onScreenLog('Connection Status Unknown ' + status);
    connectedUUIDSet.delete(device.id);

    const statusBtn = getDeviceStatusButton(device);
    statusBtn.setAttribute('class', 'device-status btn btn-outline-danger btn-sm disabled');
    statusBtn.innerText = "Error";
    getDeviceDisconnectButton(device).style.display = 'none';
    getDeviceCardBody(device).style.display = 'none';
    document.getElementById(device.id).classList.remove('active');
  }
}

function getDeviceCard(device) {
  return document.getElementById('device-' + device.id);
}

function getDeviceCardBody(device) {
  return getDeviceCard(device).getElementsByClassName('card-body')[0];
}

function getDeviceStatusButton(device) {
  return getDeviceCard(device).getElementsByClassName('device-status')[0];
}

function getDeviceDisconnectButton(device) {
  return getDeviceCard(device).getElementsByClassName('device-disconnect')[0];
}


function getDeviceNotificationButton(device) {
  return getDeviceCard(device).getElementsByClassName('notification-enable')[0];
}

function getDeviceReadData(device) {
  return getDeviceCard(device).getElementsByClassName('result-data-view')[0];
}

function renderVersionField() {
  const element = document.getElementById('sdkversionfield');
  const versionElement = document.createElement('p')
    .appendChild(document.createTextNode('SDK Ver: ' + liff._revision));
  element.appendChild(versionElement);
}

function flashSDKError(error) {
  window.alert('SDK Error: ' + error.code);
  window.alert('Message: ' + error.message);
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

function buf2hex(buffer) { // buffer is an ArrayBuffer
  return Array.prototype.map.call(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join(' ');
}
