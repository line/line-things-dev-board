# Controlling LINE Things Development Board through JavaScript

The GATT Characteristic in LINE Things development board allows easy control of the hardware and GPIO available on board.

By utilizing the Javascript library provided, you will be able to configure and set the behavior of the device purely through LIFF without the need to modify the firmware. For example, you can set the interval to which a sensor's data is sent, which cannt then be used either through LIFF or through Automatic Communication.

Please note that if the the display is controlled through this characteristic, the display will not update. To update the contents, you will need to power cycle the board or press RESET.

All of the functions explained here is available in the example `liff-app/linethings-dev-js-control`. You can use this to test out the functions before writing your own LIFF app.

Further examples are available at `liff-app/examples-js-control`.

## Before We Begin
To use this function, you will need the LINE Things Development Board firmware to be at version 2 or higher. If you get an alert in the LIFF app, please update your firmware.

See [Flashing the firmware](https://github.com/line/line-things-dev-board#updating_firmware).

## Using LIFF
The JavaScript Classes used are implemented in `things-dev-board.js`. Please make sure to require `things-dev-board.js` on your html file.
When using LIFF BLE plugin, after calling `requestDevice()`, make sure to call `connect()` on the callback return object.

To use these examples, you would need to host the pages online (such as GitHub Pages) and define their endpoints in the [Developers Console](https://developers.line.biz).

|File Name|Contents|
|----|----|
|default|Basic control of all peripherals and GPIO|
|display|OLED Example for controlling the OLED display through LIFF|
|led-blink|Controlling the LED similar to Arduino's setup() and loop()|
|notify|An example to periodically notify the user of the temperature and button presses.|

*Note* The led-blink example uses a loop in its code to replicate the behavior in Arduino. However this is poor practice since it will consume a lot of power. Please refer to this example only as a demonstration rather than guidance. A proper LED blink should be written to be event-driven to conserver power.

## JavaScript Library

### Rewriting the Service UUID
Insert the UUID you would like to set in `writeAdvertUuid(uuid)` as a string. Please note that the changes will not be applied until the next power cycle or device RESET. The LIFF application will then need to be restarted as well to reconnect to the device.

This is only available in firmware version 1.

Once set, the UUID will be stored in the Flash memory of the device. The stored UUID will be used in every subsequent power cycle. To reset the UUID to the default value, hold SW1 and press RESET.

### Reading Firmware Version
Call `deviceVersionRead()` then `versionRead()` to get the current firmware version on the device. Since firmware version 1 does not have a version read characteristic, the non-existent value of the non-existent characteristic is defaulted to 1.

The following features are available on firmware version 2 onwards.

### Display Control
Please note that the display is controlled through this characteristic and the display will not update. To display the changes you've made, you will need to power cycle the board or press RESET.

#### Clearing the Display
`displayClear()`
This is used to clear the memory buffer of the display.

#### Moving the text write cursor
`displayControl(addr_x, addr_y)`
Specifies the position to begin writing text. The range of both X and Y are 0~63.

#### Changing the font size
`displayFontSize(size)`
Specifies the font size. This can be set as 1, 2, or 3.

#### Writing Text
`displayWrite(text)`
This is used to write text (up to 16 characters) as a string. To remove the text, you will need to execute backspaces multiple times.

### LED Control
#### Individual LED Control
`ledWrite(port, value)`
Port determines the GPIO the LED is on. The board has LEDs on ports 2~5 in which are labeled as DS2~5 respectively. The values can be either 0 for OFF and 1 for ON.

#### Overall LED control
`ledWriteByte(value)`
Control all LEDs at once.

### Buzzer Control
`buzzerControl(value)`
To turn the buzzer on, set the value to 1.

### GPIO Digital Control
#### Input/Output Setting
`gpioPinMode(port, value)`
Specify the GPIO you want to set on port. Use 0 in value to set it as an INPUT and 1 to set it as an OUTPUT.

#### OUTPUT
`gpioDigitalWrite(port, value)`
Select the GPIO you want to control in port. To turn it OFF, set value to 0, to turn it on, set value to 1.

### Analog Output
`gpioAnalogWrite(port, value)`
Select the GPIO you want to control in port. Value can be set from 0~255.

### I2C
`i2cStartTransmission(address)`
`i2cWrite(value)`
`i2cStopTransmission()`
`i2cRequestFrom(address, length)`
`i2cReadRequest(device)`

These are designed to work similar to Arduino's I2C. `i2cReadRequest(device)` is used to read data from an I2C device and storing it into a buffer on the dev board. The data can then be read via LIFF through `readReq(cmd)` and `deviceRead()`.

### Reading Data from the Device
To read data from the device, you will first need to store the data into the device's buffer. Specify the port of the target peripheral by peripheral type, and the port number to read. Note that you will not need to specify these for switches, accelerometer, and temperature sensor since `readReq(cmd)` provides the read methods already.

`i2cReadRequest(device)`
`gpioDigitalReadReq(port)`
`gpioAnalogReadReq(port)`

To read data and store it into buffer call `readReq(cmd)`


|cmd value|Data type|
----|----
|0|Switch|
|1|Accelerometer|
|2|Temperature|
|3|Digital GPIO value|
|4|Analog GPIO value|
|5|I2C Data|

Finally, call `deviceRead()` to read the data stored in the buffer.

### Notification
#### Switch
`swNotifyEnable(source, mode, interval, callback)`
Enables switch state notification based on the mode set.
`swNotifyDisable()`
Disables switch notification
##### Source

|Value|Source|
----|----
|0|disable|
|1|SW1|
|2|SW2|
|3|SW1 or SW2|

##### Mode

|Value|State|
----|----
|0|LOW|
|1|CHANGE|
|2|RISING|
|3|FALLING|

##### Interval
Specifies the time in milliseconds before the next interrupt is allowed to be made. This can be use a button debounce.

##### Callback
Let's you set a method to be executed when a notification is triggered.

#### Temperature
`tempNotifyEnable(interval, callback)`
Enables notification of the temperature reading regularly.
`tempNotifyDisable()`
Disables the temperature notification.
Please note that `i2cStartTransmission(address)` is called when the notification is enabled and will not stop until `i2cStopTransmission()` is explicitly called.

##### Source

|Value|State|
----|----
|0|disable|
|1|enable|

##### callback
Let's you set a method to be executed when a notification is triggered.
