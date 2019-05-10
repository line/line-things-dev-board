
class ThingsConn {
    constructor(device, svUuid, versionUuid, writeUuid, writeIoUuid, readIoUuid, notifySwUuid, notifyTempUuid) {
        this.device = device;
        this.svUuid = svUuid;
        this.verUuid = versionUuid;
        this.wrUuid = writeUuid;
        this.wrIoUuid = writeIoUuid;
        this.rdIoUuid = readIoUuid;
        this.ntfySwUuid = notifySwUuid;
        this.ntfyTempUuid = notifyTempUuid;
    }

    async writeAdvertUuid(uuid){
        const tx_uuid = uuid.replace(/-/g, '');
        let uuid_byte = [];
        let hash = 0;
        for(let i = 0; i < 16; i = i + 1) {
            uuid_byte[i] = parseInt(tx_uuid.substring(i * 2, i * 2 + 2), 16);
            hash = hash + uuid_byte[i];
        }

        const header = [1, 0, 0, hash];
        const command = header.concat(uuid_byte);
        await this.writeCharacteristic(command, 'control').catch(e => onScreenLog(`error : writeAdvertUuid()`));
    }

    async tempNotifyEnable(interval, callback){
        const notifyCharacteristic = await this.getCharacteristic(
            this.device, this.svUuid, this.ntfyTempUuid);

        notifyCharacteristic.addEventListener('characteristicvaluechanged', callback);
        await notifyCharacteristic.startNotifications().catch(e => {
            onScreenLog('notify start error');
            return null;
        });
        onScreenLog('Temperature Notifications STARTED ' + notifyCharacteristic.uuid);

        const command = [18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, interval >> 8, interval & 0xff];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : tempNotifyEnable()`));
    }

    async tempNotifyDisable(){
        const command = [18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : tempNotifyDisable()`));

        const notifyCharacteristic = await this.getCharacteristic(
            this.device, this.svUuid, this.ntfyTempUuid);

        notifyCharacteristic.removeEventListener('characteristicvaluechanged', callback);
        await notifyCharacteristic.stopNotifications().catch(e => onScreenLog(`error : tempNotifyDisable()`));
        onScreenLog('Temperature Notifications STOP ' + notifyCharacteristic.uuid);
    }

    async swNotifyEnable(source, mode, interval, callback){
        const notifyCharacteristic = await this.getCharacteristic(
            this.device, this.svUuid, this.ntfySwUuid);

        notifyCharacteristic.addEventListener('characteristicvaluechanged', callback);
        await notifyCharacteristic.startNotifications().catch(e => {
            onScreenLog('notify start error');
            return null;
        });
        onScreenLog('SW Notifications STARTED ' + notifyCharacteristic.uuid);

        const command = [17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, source, mode, interval >> 8, interval & 0xff];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error swNotifyEnable()`));
    }

    async swNotifyDisable(){
        const command = [17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : swNotifyDisable()`));

        const notifyCharacteristic = await this.getCharacteristic(
            this.device, this.svUuid, this.ntfySwUuid);

        notifyCharacteristic.removeEventListener('characteristicvaluechanged', callback);
        await notifyCharacteristic.stopNotifications().catch(e => onScreenLog(`error : swNotifyDisable()`));
        onScreenLog('SW Notifications STOP ' + notifyCharacteristic.uuid);
    }

    async displayWrite(text) {
        let ch_array = text.split("");
        for(let i = 0; i < 16; i = i + 1){
            if(i >= text.length){
                ch_array[i] = 0;
            }else{
                ch_array[i] = (new TextEncoder('ascii')).encode(ch_array[i]);
            }
        }
        const cmd = [1, text.length];
        const command = cmd.concat(ch_array);
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : displayWrite()`));
    }

    async displayControl(addr_x, addr_y) {
        const command = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, addr_x, addr_y];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : displayControl()`));
    }

    async displayFontSize(size) {
        const command = [15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, size];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : displayFontSize()`));
    }

    async displayClear() {
        const command = [2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : displayClear()`));
    }

    async ledWrite(port, value) {
        const command = [3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, value];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : ledWrite()`));
    }

    async ledWriteByte(value) {
        const command = [16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, value];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : ledWriteByte()`));
    }

    async buzzerControl(value) {
        const command = [4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, value];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : buzzerControl()`));
    }

    async gpioPinMode(port, value) {
        const command = [5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, value];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : gpioPinMode()`));
    }

    async gpioDigitalWrite(port, value) {
        const command = [6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, value];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : gpioDigitalWrite()`));
    }

    async gpioAnalogWrite(port, value) {
        const command = [7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, value];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : gpioAnalogWrite()`));
    }

    async i2cStartTransmission(address) {
        const command = [8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, address];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : i2cStartTransmission()`));
    }

    async i2cWrite(value) {
        const command = [9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, value];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : i2cWrite()`));
    }

    async i2cStopTransmission() {
        const command = [10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : i2cStopTransmission()`));
    }

    async i2cRequestFrom(address, length) {
        const command = [11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, length, address];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : i2cRequestFrom()`));
    }

    async i2cReadRequest(device) {
        const command = [12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : i2cReadRequest()`));
    }

    async gpioDigitalReadReq(port) {
        const command = [13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : gpioDigitalReadReq()`));
    }

    async gpioAnalogReadReq(port) {
        const command = [14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : gpioAnalogReadReq()`));
    }

    async readReq(cmd) {
        const command = [32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, cmd];
        await this.writeCharacteristic(command, 'io').catch(e => onScreenLog(`error : readReq()`));
    }

    async deviceRead() {
        const readCmdCharacteristic = await this.getCharacteristic(
            this.device, this.svUuid, this.rdIoUuid).catch(e => onScreenLog(`error : deviceRead(), getCharacteristic`));

        const valueBuffer = await this.readCharacteristic(readCmdCharacteristic).catch(e => {
            onScreenLog('Read Value  : ' + "error");
            return null;
        });

        const result = [valueBuffer.getInt16(0, true), valueBuffer.getInt16(2, true)];
        onScreenLog('Read Value  : ' + result[0] + ", " + result[1]);
        this.readvalue = (result[0] * 65536) + result[1];
    }


    async deviceVersionRead() {
        const readCmdCharacteristic = await this.getCharacteristic(
            this.device, this.svUuid, this.verUuid).catch(e => onScreenLog(`error : deviceVersionRead(), getCharacteristic`));

        const valueBuffer = await this.readCharacteristic(readCmdCharacteristic).catch(e => {
            onScreenLog('error : deviceVersionRead()');
            this.version = 1;
            return null;
        });

        const result = valueBuffer.getInt8(0);
        onScreenLog('Version : ' + result);
        this.version = result;
    }

    versionRead(){
        return this.version;
    }

    valueRead(){
        return this.readvalue;
    }

    async sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    async readCharacteristic(characteristic) {
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

    async writeCharacteristic(data, mode){
        let uuid;
        if(mode == 'control'){
            uuid = this.wrUuid;
        }else if(mode == 'io'){
            uuid = this.wrIoUuid;
        }else{
            return;
        }
        const characteristic = await this.getCharacteristic(
              this.device, this.svUuid, uuid);
        await characteristic.writeValue(new Uint8Array(data)).catch(e => {
            onScreenLog(`Write value to device ${characteristic.uuid}: ${e}`);
            throw e;
        });
        await this.sleep(50);
    }

    async getCharacteristic(device, serviceId, characteristicId) {
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
}
