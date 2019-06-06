# Flashing the Firmware
The method below shows how to flash the firmware using Arduino IDE

## Arduino Environment Preparation
After installing Arduino IDE, proceeed with the following steps. Please have the board **disconnected** during this process.

1. Open Arduino IDE
2. Open Preferences
3. Add `https://www.adafruit.com/package_adafruit_index.json` at 'Additional Board Manager URL'
4. Go to **Tools -> Board** and select **Boards Manager**
5. Search for "nRF52" and install **Adafruit nRF52 by Adafruit** (version 0.10.1 or higher) (*Note: If you are using Linux, additional configuration is required, see [Linux Setup](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/arduino-bsp-setup)*)
6. Install [CP2102N driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)

## Setting Up the Hardware
### Using CPU core with the daughter board

1. Ensure that the CPU core is proper installed onto the daughter board.
2. Connect the board to your PC using a Micro-USB cable connected to J1.
3. Go to **Tools -> Board** and select **Adafruit Bluefruit Feather nRF52832**.
4. Go to **Tools -> Board** and make sure the correct port is selected *ie. COM1, /dev/cu.SLAB_USBtoUART*.
5. To test the environment, click upload to upload an empty sketch onto the device. There should be no errors.

### Using the CPU core as a stand-alone via daughter board' Serial IC

1. Connect pins *RxD*/*TxD*/*Reset*/*VCC*/*GND* from the CPU core to the daughter board as shown below. For pin assignments, refer to the CPU core pin assignment.
2. Connect the daughter board to a PC using a Micro-USB cable connected to J1.
3. Go to **Tools -> Board** and select **Adafruit Bluefruit Feather nRF52832**.
4. Go to **Tools -> Board** and make sure the correct port is selected *ie. COM1, /dev/cu.SLAB_USBtoUART*.
5. To test the environment, click upload to upload an empty sketch onto the device. There should be no errors.

![motherboard_external](https://user-images.githubusercontent.com/135050/58088646-14a86280-7bfe-11e9-974d-d4925148f3ae.jpg)

### Using the CPU core as a stand-alone via external Serial ICs (not recommended)
Any Serial IC with USB-TTL conversion that can output DTR as well (such as the CP2102 mounted on the daughter board) can be used. In addition to the adapter, a 0.1uF capacitor is required.

1. Connect pins *RxD*/*TxD*/*Reset*/*VCC*/*GND* from the CPU core to the Serial adapter as shown below. **Ensure the capacitor is connected between the Reset pin and the DTR pin**
2. Connect the Serial adapter to a PC using a Micro-USB cable connected to J1.
3. Go to **Tools -> Board** and select **Adafruit Bluefruit Feather nRF52832**.
4. Go to **Tools -> Board** and make sure the correct port is selected *ie. COM1, /dev/cu.SLAB_USBtoUART*.
5. To test the environment, click upload to upload an empty sketch onto the device. There should be no errors. 


| USB-TTL IC | CPU Core | Important Remarks |
----|----|----
| VDD(3.3V) | P28 | |
| GND | P1 | |
| DTR | P27 | 間に0.1uFのコンデンサを挟んでください |
| RxD | P6 | |
| TxD | P8 | |

![cpuboard_prog_external](https://user-images.githubusercontent.com/135050/58088653-18d48000-7bfe-11e9-9b5a-f9637b23e590.jpg)

## Advanced Users
### Using a J-LINK
By using a J-LINK, you are able to perform advanced development of the device without Arduino IDE. Connect the debugger to the CPU core as show in the picture below. Please note that using a J-LINK debugger will overwrite the original bootloader, thus making the board no longer compatible with Arduino IDE. Modifications to the CPU core on a lower level can invalidate any design certification placed on the device. Please proceed at your own risk if you are aware of what you are doing. Should you need to restore the CPU core back to its initial state (compatible with Arduino IDE), see [Using Adafruit's Bootloader](#Using-Adafruit;s-Bootloader).

#### Wiring Diagram
![cpuboard_jlink](https://user-images.githubusercontent.com/135050/58088636-1114db80-7bfe-11e9-8a03-2222d94d49c5.png)

| Connect Pin Number | Pin Name |
----|----|
| 1 | SWDIO |
| 2 | GND |
| 3 | SWDCLK |
| 4 | VCC |


### Using Adafruit's Bootloader
Should you need to revert your board back to the original bootloader, you will need a J-LINK debugger to burn the bootloader to the board.

Please follow Adafruit's guide [Arduino Core for Adafruit Bluefruit nRF52 Boards - Burning new Bootloader](https://github.com/adafruit/Adafruit_nRF52_Arduino/#burning-new-bootloader) to burn the bootloader.

※ Do **NOT** do this, if your board is already working with Arduino IDE. If you would like to update the bootloader, see [Updating the BootLoader](#Updating-the-BootLoader) 

### Updating the BootLoader
Adafruit has a comprehensive guide to performing a bootloader update at 
[Updating the Bootloader](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/updating-the-bootloader).