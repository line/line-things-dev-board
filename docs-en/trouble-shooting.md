# Troubleshooting
## I am unable to detect the device / I cannot connect to the device
The cache of smartphone may store incorrect or corrupt data. Normally toggling Bluetooth off and on from the settings menu should resolve the issue, however, a restart may be required.

## I am unable to connect/program through Arduino. I am getting errors in Arduino
In Arduino IDE, go to `Tools -> Board` and make sure `Adafruit Bluefruit nRF52832 Feather` is selected. If you do not have this option available, please refer to the Flashing Firmware documentation.
If you continue to get errors, make sure that under `Tools -> Port` the correct port is selected for your device.

If errors still occur, there may be issues with your bootloader or SoftDevice, such as inconsistent versions or incompatible versions. To fix this refer to Adafruit's Documentation, [Arduino Core for Adafruit Bluefruit nRF52 Boards - Bootloader Support](https://github.com/adafruit/Adafruit_nRF52_Arduino/#bootloader-support).

## POWER LED(*DS1*) does not light up
Ensure that the jumper (*J4*) is set according to the power source preferred. Double check the polarity of your power source and orientation of the batteries to make sure no connections are reversed.

## The motor isn't moving
Originally, the development board does ont come with the necessary parts to drive motors. You will need to install those parts yourself, see [Motor driver](#Motor_driver) for more details. If the parts are already installed, ensure that all connections are properly made between all components.

Please bear in mind that the battery power source is not enough to drive the motors. An external power source is highly recommended.

## Motor moves but seems unstable
The GND pad on the under side of the DRV8830DGQR IC may not be soldered. Make sure to solder it via the through-hole connection from the bottom side of the dev board.

## Something was originally working then stopped working while programming/debugging
If you are using I2C devices (the OLED display, temperature sensor, accelerometer on the board), their communications may have been deadlocked. Resetting the development board will not resolve the issue, so you will need to power cycle the board.

## Arduino cannot compile the sample files
In Arduino, go to `Tools -> Board` and open **Boards Manager**. Search for "nRF52" and find **Adafruit nRF52 by Adafruit**. Make sure that the version installed is **0.10.1** or higher.

## I've set a new UUID but the advertised UUID remains unchanged
The default sample firmware stores the UUID in Flash memory. In order to make changes to the UUID, the UUID changes must be applied after the device boots up the firmware and override the preloaded UUID value.

See [Reverting Service UUID to Stock](#service-uuid-reset)

Once the new UUID has been set, on the smartphone side, you will need to unpair with the device in the Bluetooth settings as well as unlink the product from LINE Things Device Link screen. This should allow you to connect to the device with its new UUID. If there are still issues, repeat the steps above, then kill the LINE application and toggle the Bluetooth off and on from your device settings. A restart may be required.
