# Default Firmware

## GATT Service Specifications

This section describes the specifications of GATT service characteristic implemented in default firmware. The details here are not necessary when using the [JavaScript Library](js-control.md) but is crucial for extending or controlling the device from other sources than LIFF.

### Details of Characteristics
All Characteristics except PSDI are implemented in the following service:

- `DEVBOARD_SERVICE_UUID`
  - `f2b742dc-35e3-4e55-9def-0ce4a209c552`

The Characteristics and their UUIDs within this service are described below.

|Description | Characteristic Name | UUID | Properties | Size (Fixed Length) |
----|----|----|----|----
| 1 | WRITE_BOARD_STATE_CHARACTERISTIC_UUID | 4f2596d7-b3d6-4102-85a2-947b80ab4c6f | Write | 20Byte |
| 2 | VERSION_CHARACTERISTIC_UUID | be25a3fe-92cd-41af-aeee-0a9097570815 | Read | 1Byte |
| 3 | NOTIFY_SW_CHARACTERISTIC_UUID | a11bd5c0-e7da-4015-869b-d5c0087d3cc4 | Notify | 2Byte |
| 4 | NOTIFY_TEMP_CHARACTERISTIC_UUID | fe9b11a8-5f98-40d6-ae82-bea94816277f | Notify | 2Byte |
| 5 | COMMAND_WRITE_CHARACTERISTIC_UUID | 5136e866-d081-47d3-aabc-a2c9518bacd4 | Write | 18Byte |
| 6 | COMMAND_RESPONSE_CHARACTERISTIC_UUID | 1737f2f4-c3d3-453b-a1a6-9efe69cc944f  | Read + Notify | 4Byte |

1. This is used for reqriting the Service UUID in the JavaScript library.
2. This is the firmware version of the Dev Board. If undefined, the version will be defaulted to 1.
3. When subscribed, this will notify the switch state upon change.
4. When subscribed, this will notify the current temperature at regular intervals.
5. This is used to write commands to the device, such as device settings and notify settings.
6. If a command has a response, the response will be sent/can be read from this characteristic.

### WRITE_BOARD_STATE_CHARACTERISTIC_UUID (Rewriting the Service UUID)
`Write device` allows values to be written to the board as shown in `/liff-app/linethings-dev-default`. However, this characteristic is exclusively used for rewriting the Service UUID.

Protocol

| |Command (1Byte) | reserved (2Byte) | hash (1Byte) | payload (16Byte) | Description |
----|----|----|----|----|----
| Write device | 0 | {0, 0} | 0 | see code | Implemented in Version 1 onwards, this is used for setting up devices and GPIO.|
| Write Service UUID | 1 | {0, 0} | hash of uuid | uuid | UUID payload omits "-"|

### VERSION_CHARACTERISTIC_UUID (Reading Firmware Version)
This characteristic is for reading the firmware version. Firmware version 1 does not have this characteristic implemented. Therefore, if this characteristic has an error in LIFF, you can safely assume you are using firmware version 1. All other functions will continue to work other than Rewriting the Service UUID implemented in version 2 onwards.

### NOTIFY_SW_CHARACTERISTIC_UUID (SW Notify)
This sends notifications when the switch is pressed. The switch notification is set at `DEFAULT_CHARACTERISTIC_IO_WRITE_UUID` by default.
Other default settings are as follows.
- Source is from SW1 and SW2
- Mode is `CHANGE`
- Interval is set at 50ms

Protocol

|value[1]|value[0]|
----|----
|address(1:SW1, 2:SW2)|SW value|

### NOTIFY_TEMP_CHARACTERISTIC_UUID (Temperature Notify)
`DEFAULT_CHARACTERISTIC_IO_WRITE_UUID` is the default characteristic set to used for temperature notification. If subscribed, the temperature data of 16 bytes will be sent 100 times, once every 10 seconds by default.
Initial setting: source = 1, interval = 10000ms

### COMMAND_WRITE_CHARACTERISTIC_UUID (Device Usage)
This is used to configure various settings for the peripherals on board.

Protocol

| |Command (1Byte) | Payload (17Byte) | Description |
----|----|----|----
| Control display | 0 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, address_x, address_y} | Specify the index address for Write Text | |
| Write text | 1 | {length of text, (text max 16Byte)} | Writes any text | 
| Clear display| 2| N/A | Clears the Display| |
| Write LED | 3 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, value} |  |
| Buzzer | 4 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, value(0:OFF / 1:ON)} |  |
| GPIO Direction | 5 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, direction(0:Input / 1:Output)} |  |
| GPIO Digital Write  | 6 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, value} |  |
| GPIO Analog Write  | 7 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port, value} |  |
| I2C Start transmission | 8 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, address} |  |
| I2C Write data | 9 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, data} |  |
| I2C Stop transmission | 10 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} |  |
| I2C Request from | 11 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, address} |  |
| I2C Read request | 12 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} |  |
| Set digital GPIO port for read | 13 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port} |  |
| Set analog GPIO port for read | 14 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, port} |  |
| Set display font size| 15 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, fontsize(1~3)} |  |
| Write LED Byte | 16 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, value} |  |
| SW Notify config | 17 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, source(0:Disable / 1:SW1, 2:SW2, 3:SW1 & SW2), mode(0:LOW / 1:CHANGE / 2:RISING / 3:FALLING), interval[1], interval[0]} |  |
| Temperature Notify config | 18 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, source(0:Disable / 1:Temperature), interval[1], interval[0]} |  |
| Read value request | 32 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, source(0:SW / 1:Accel / 2:Temp / 3:DIgital GPIO / 4:Analog GPIO / 5:I2C)} |  |

### COMMAND_RESPONSE_CHARACTERISTIC_UUID (Read via LINE)
`COMMAND_WRITE_CHARACTERISTIC_UUID` is the characteristic to read data and states of peripherals and GPIO. The data received will depend on the peripheral requested.

|Source|Value format|
----|----
|Switch|{0, 0, SW2, SW1}|
|Accel| {0, X, Y, Z} |
|Temperature|温度が100倍された値 |
|GPIO Digital| {0, 0, 0, value} |
|GPIO Analog| {0, 0, 0, value}|
|I2C| {0, 0, (0:invalid / 1:valid), value} |