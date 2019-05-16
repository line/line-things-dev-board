const USER_SERVICE_UUID = "f2b742dc-35e3-4e55-9def-0ce4a209c552";
const USER_CHARACTERISTIC_NOTIFY_UUID = "e90b4b4e-f18a-44f0-8691-b041c7fe57f2";
const USER_CHARACTERISTIC_WRITE_UUID = "4f2596d7-b3d6-4102-85a2-947b80ab4c6f";

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
    // onScreenLog("Check availablity: " + isAvailable);

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
    // onScreenLog('detect: ' + device.id);

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
        try {
            connectDevice(device);
        } catch (e) {
            onScreenLog('Initializing device failed. ' + e);
        }
    });
    deviceList.appendChild(deviceItem);
}

// Select target device and connect it
function connectDevice(device) {
    onScreenLog('Device selected: ' + device.name);

    if (!device) {
        onScreenLog('No devices found. You must request a device first.');
    } else if (connectingUUIDSet.has(device.id) || connectedUUIDSet.has(device.id)) {
        onScreenLog('Already connected to this device.');
    } else {
        connectingUUIDSet.add(device.id);
        initializeCardForDevice(device);

        // Wait until the requestDevice call finishes before setting up the disconnect listner
        const disconnectCallback = () => {
            updateConnectionStatus(device, 'disconnected');
            device.removeEventListener('gattserverdisconnected', disconnectCallback);
        };
        device.addEventListener('gattserverdisconnected', disconnectCallback);

        onScreenLog('Connecting ' + device.name);
        device.gatt.connect().then(() => {
            updateConnectionStatus(device, 'connected');
            connectingUUIDSet.delete(device.id);
        }).catch(e => {
            flashSDKError(e);
            onScreenLog(`ERROR on gatt.connect(${device.id}): ${e}`);
            updateConnectionStatus(device, 'error');
            connectingUUIDSet.delete(device.id);
        });
    }
}

// Setup device information card
function initializeCardForDevice(device) {
    const template = document.getElementById('device-template').cloneNode(true);
    const cardId = 'device-' + device.id;

    template.style.display = 'block';
    template.setAttribute('id', cardId);
    template.querySelector('.card > .card-header > .device-name').innerText = device.name;

    //LED Control
    template.querySelector('.led0_value').addEventListener('change', () => {
        onScreenLog('LED0 Value changed');
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));

        const led0_status = template.querySelector('.led0_value').checked;
        onScreenLog('LED0 status : ' + led0_status);

    });
    template.querySelector('.led1_value').addEventListener('change', () => {
        onScreenLog('LED1 Value changed');
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.led2_value').addEventListener('change', () => {
        onScreenLog('LED2 Value changed');
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.led3_value').addEventListener('change', () => {
        onScreenLog('LED3 Value changed');
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.buzzer_value').addEventListener('change', () => {
        onScreenLog('Buzzer Value changed');
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io2_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io3_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io4_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io5_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io12_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io13_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io14_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io15_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });
    template.querySelector('.io16_value').addEventListener('change', () => {
        updateLedState(device).catch(e => onScreenLog(`ERROR on updateLedState(): ${e}\n${e.stack}`));
    });

    // Device disconnect button
    template.querySelector('.device-disconnect').addEventListener('click', () => {
        onScreenLog('Clicked disconnect button');
        device.gatt.disconnect();
    });

    template.querySelector('.setuuid').addEventListener('click', () => {

        writeAdvertuuid(device, template.querySelector('.uuid_text').value).catch(e => onScreenLog(`ERROR on writeAdvertuuid(): ${e}\n${e.stack}`));
    });


    template.querySelector('.notification-enable').addEventListener('click', () => {
        toggleNotification(device).catch(e => onScreenLog(`ERROR on toggleNotification(): ${e}\n${e.stack}`));
    });

    // Tabs
    ['notify', 'write', 'advert'].map(key => {
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

async function toggleNotification(device) {
    if (!connectedUUIDSet.has(device.id)) {
        window.alert('Please connect to a device first');
        onScreenLog('Please connect to a device first.');
        return;
    }

    const accelerometerCharacteristic = await getCharacteristic(
        device, USER_SERVICE_UUID, USER_CHARACTERISTIC_NOTIFY_UUID);

    if (notificationUUIDSet.has(device.id)) {
        // Stop notification
        await stopNotification(accelerometerCharacteristic, notificationCallback);
        notificationUUIDSet.delete(device.id);
        getDeviceNotificationButton(device).classList.remove('btn-success');
        getDeviceNotificationButton(device).classList.add('btn-secondary');
        getDeviceNotificationButton(device).getElementsByClassName('fas')[0].classList.remove('fa-toggle-on');
        getDeviceNotificationButton(device).getElementsByClassName('fas')[0].classList.add('fa-toggle-off');
    } else {
        // Start notification
        await enableNotification(accelerometerCharacteristic, notificationCallback);
        notificationUUIDSet.add(device.id);
        getDeviceNotificationButton(device).classList.remove('btn-secondary');
        getDeviceNotificationButton(device).classList.add('btn-success');
        getDeviceNotificationButton(device).getElementsByClassName('fas')[0].classList.remove('fa-toggle-off');
        getDeviceNotificationButton(device).getElementsByClassName('fas')[0].classList.add('fa-toggle-on');
    }
}

async function enableNotification(characteristic, callback) {
    const device = characteristic.service.device;
    characteristic.addEventListener('characteristicvaluechanged', callback);
    await characteristic.startNotifications();
    onScreenLog('Notifications STARTED ' + characteristic.uuid + ' ' + device.id);
}

async function stopNotification(characteristic, callback) {
    const device = characteristic.service.device;
    characteristic.removeEventListener('characteristicvaluechanged', callback);
    await characteristic.stopNotifications();
    onScreenLog('Notifications STOPPED　' + characteristic.uuid + ' ' + device.id);
}

function notificationCallback(e) {
    const accelerometerBuffer = new DataView(e.target.value.buffer);
    onScreenLog(`Notify ${e.target.uuid}: ${buf2hex(e.target.value.buffer)}`);
    updateSensorValue(e.target.service.device, accelerometerBuffer);
}

async function refreshValues(device) {
    const accelerometerCharacteristic = await getCharacteristic(
        device, USER_SERVICE_UUID, USER_CHARACTERISTIC_NOTIFY_UUID);

    const accelerometerBuffer = await readCharacteristic(accelerometerCharacteristic).catch(e => {
        return null;
    });

    if (accelerometerBuffer !== null) {
        updateSensorValue(device, accelerometerBuffer);
    }
}

function updateSensorValue(device, buffer) {
    const temperature = buffer.getInt16(0, true) / 100.0;
    const accelX = buffer.getInt16(2, true) / 1000.0;
    const accelY = buffer.getInt16(4, true) / 1000.0;
    const accelZ = buffer.getInt16(6, true) / 1000.0;
    const sw1 = buffer.getInt16(8, true);
    const sw2 = buffer.getInt16(10, true);

    getDeviceProgressBarX(device).style.width = (accelX / 4 * 100 + 50) + "%";
    getDeviceProgressBarY(device).style.width = (accelY / 4 * 100 + 50) + "%";
    getDeviceProgressBarZ(device).style.width = (accelZ / 4 * 100 + 50) + "%";
    getDeviceProgressBarTemperature(device).innerText = temperature + "℃";
    getDeviceProgressBarX(device).innerText = accelX;
    getDeviceProgressBarY(device).innerText = accelY;
    getDeviceProgressBarZ(device).innerText = accelZ;
    getDeviceStatusSw1(device).innerText = (sw1 == 0x0001)? "ON" : "OFF";
    getDeviceStatusSw2(device).innerText = (sw2 == 0x0001)? "ON" : "OFF";
}

async function readCharacteristic(characteristic) {
    const response = await characteristic.readValue().catch(e => {
        onScreenLog(`Error reading ${characteristic.uuid}: ${e}`);
        throw e;
    });
    if (response) {
        onScreenLog(`Read ${characteristic.uuid}: ${buf2hex(response.buffer)}`);
        const values = new DataView(response.buffer);
        return values;
    } else {
        throw 'Read value is empty?';
    }
}

async function updateLedState(device) {
    const card = getDeviceCard(device);
    const led0_status = !!card.querySelector('.led0_value').checked;
    const led1_status = !!card.querySelector('.led1_value').checked;
    const led2_status = !!card.querySelector('.led2_value').checked;
    const led3_status = !!card.querySelector('.led3_value').checked;
    const buzzer_status = !!card.querySelector('.buzzer_value').checked;
    const io2_status = !!card.querySelector('.io2_value').checked;
    const io3_status = !!card.querySelector('.io3_value').checked;
    const io4_status = !!card.querySelector('.io4_value').checked;
    const io5_status = !!card.querySelector('.io5_value').checked;
    const io12_status = !!card.querySelector('.io12_value').checked;
    const io13_status = !!card.querySelector('.io13_value').checked;
    const io14_status = !!card.querySelector('.io14_value').checked;
    const io15_status = !!card.querySelector('.io15_value').checked;
    const io16_status = !!card.querySelector('.io16_value').checked;

    let data = [0, 0];
    data[1] = data[1] + (led0_status << 7);
    data[1] = data[1] + (led1_status << 6);
    data[1] = data[1] + (led2_status << 5);
    data[1] = data[1] + (led3_status << 4);
    data[1] = data[1] + (buzzer_status << 3);
    data[1] = data[1] + (io2_status << 2);
    data[1] = data[1] + (io3_status << 1);
    data[1] = data[1] + io4_status;
    data[0] = data[0] + (io5_status << 7);
    data[0] = data[0] + (io12_status << 6);
    data[0] = data[0] + (io13_status << 5);
    data[0] = data[0] + (io14_status << 4);
    data[0] = data[0] + (io15_status << 3);
    data[0] = data[0] + (io16_status << 2);

    const header = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    const command = header.concat(data);

    onScreenLog('Write to device : ' + new Uint8Array(command));

    const characteristic = await getCharacteristic(
          device, USER_SERVICE_UUID, USER_CHARACTERISTIC_WRITE_UUID);
    await characteristic.writeValue(new Uint8Array(command)).catch(e => {
        onScreenLog(`Error writing ${characteristic.uuid}: ${e}`);
        throw e;
    });
}

async function writeAdvertuuid(device, uuid) {
  const tx_uuid = uuid.replace(/-/g, '');
  let uuid_byte = [];
  let hash = 0;
  for(let i = 0; i < 16; i = i + 1) {
    uuid_byte[i] = parseInt(tx_uuid.substring(i * 2, i * 2 + 2), 16);
    hash = hash + uuid_byte[i];
  }

  const header = [1, 0, 0, hash];
  const command = header.concat(uuid_byte);

  onScreenLog('Write new advert UUID to device  : ' + new Uint8Array(command));

  const characteristic = await getCharacteristic(
        device, USER_SERVICE_UUID, USER_CHARACTERISTIC_WRITE_UUID);
  await characteristic.writeValue(new Uint8Array(command)).catch(e => {
      onScreenLog(`Error writing ${characteristic.uuid}: ${e}`);
      throw e;
  });
}


async function getCharacteristic(device, serviceId, characteristicId) {
    const service = await device.gatt.getPrimaryService(serviceId).catch(e => {
        flashSDKError(e);
        throw e;
    });
    const characteristic = await service.getCharacteristic(characteristicId).catch(e => {
        flashSDKError(e);
        throw e;
    });
    onScreenLog(`Got characteristic ${serviceId} ${characteristicId} ${device.id}`);
    return characteristic;
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

function getDeviceProgressBarTemperature(device) {
    return getDeviceCard(device).getElementsByClassName('progress-bar-temperature')[0];
}

function getDeviceProgressBarX(device) {
    return getDeviceCard(device).getElementsByClassName('progress-bar-x')[0];
}

function getDeviceProgressBarY(device) {
    return getDeviceCard(device).getElementsByClassName('progress-bar-y')[0];
}

function getDeviceProgressBarZ(device) {
    return getDeviceCard(device).getElementsByClassName('progress-bar-z')[0];
}

function getDeviceStatusSw1(device) {
    return getDeviceCard(device).getElementsByClassName('sw1-value')[0];
}
function getDeviceStatusSw2(device) {
    return getDeviceCard(device).getElementsByClassName('sw2-value')[0];
}

function getDeviceNotificationButton(device) {
    return getDeviceCard(device).getElementsByClassName('notification-enable')[0];
}

function renderVersionField() {
    const element = document.getElementById('sdkversionfield');
    const versionElement = document.createElement('p')
        .appendChild(document.createTextNode('SDK Ver: ' + liff._revision));
    element.appendChild(versionElement);
}

function flashSDKError(error){
    window.alert('SDK Error: ' + error.code);
    window.alert('Message: ' + error.message);
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function buf2hex(buffer) { // buffer is an ArrayBuffer
    return Array.prototype.map.call(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join('');
}
