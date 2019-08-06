# LINE Things Development Board
The LINE Things development board is powered by [Nordic Semiconductor's nRF52832] (https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52832), in which the specific module used is the [Raytac MDBT42Q] (http: // www.raytac.com/product/ins.php?index_id=31). This development board is for experimenting with LINE Things. It actually consists of two boards, the first being the core or the CPU board, and the other is the peripheral board or the daughterboard. After verifying the functionality of a concept through the daughterboard, the core can be removed to be placed into a breadboard for custom board designs and prototyping.

The [Arduino library open sourced by Adafruit] (https://github.com/adafruit/Adafruit_nRF52_Arduino) is used here to allow beginners and veterans to easily experiment with LINE Things through Arduino IDE.

This document provides details on how to use each board and the peripherals mounted on the boards.
Please refer to each page about how to use LINE Things and Bluetooth LE.

-[LINE Things-LINE Developers] (https://developers.line.biz/en/docs/line-things/)
-[LINE Hands on dev board use] (https://qiita.com/cpulabs/items/3a37a70cfa41129cb024)
-[Try trial of LINE's IoT platform LINE Things Developer Trial] (https://engineering.linecorp.com/en/blog/line-things-developer-trial/)
-[LINE Things automatic communication function has been released & how to use it] (https://engineering.linecorp.com/en/blog/line-things-automatic-communication/)

### [Important] Notes on using the Grove connector

The main board itself will function normally as it is, however there are a couple of mislabeled parts concerning the **Grove connectors on board**.

* Labels of **P2** and **P6** on the board are swapped. The correct labels should be, **P2** is I2C (5V) and **P6** is I2C (3.3V).
* Labels of **P7** and **P8** on the board are swapped. The correct labels should be, **P7** is Grove-I/O and **P8** is Grove-UART.

## Table of Contents

[Documentation on Github Pages] (https://line.github.io/line-things-dev-board/)

-README.md
  -[Quick Start] (#quick-start)
  -[How to use] (#How to use)
  -[Precautions] (#Precautions)
  -[Hardware Specification] (#Hardware Specification)
  -[Extension] (#Extension)
  -[Schematics and Board Design] (#Schematics and Board Design)
-Documents
  -[Default Firmware] (docs-en/default-firmware.md)
  -[Controlling LINE Things Development Board through JavaScript] (docs-en/js-control.md)
  -[Flashing Firmware] (docs-en/update-firmware.md)
  -[Sample Code for Arduino] (docs-en/examples.md)
  -[Troubleshooting] (docs-en/trouble-shooting.md)

## Quick Start
With the default firmware, you can experiment with LINE Things and all of the on-board peripherals right away.
You will be able to view the statuses of all the sensors, as well as control the LEDs.
Upon powering the board on and connecting to it via LINE Things, you will be able to Write values to the device and send Notify events back to LINE app.
Please note that the default TX power setting is set to the minimum setting, so you will need to be physically close to the device.

### For the first time using LINE Things
If this is the first time you are using LINE Things, please scan the QR code founnd on the daughter board or on the under side of the CPU board. You will need to agree to the terms of use in order to use LINE Things.

! [] (https://developers.line.biz/media/line-things/qr_code-311f3503.png)

### Using the Default Firmware
The default firmware provides control of the development board through LIFF without the need to make any modifications to the firmware. For example, it is possible to send notifications from the development board with values from the sensors etc.
A library, utilizing LIFF's Javascript API, is also provided to make it easy to interact wioth the default firmware.

For the default firmware specifications, refer to [Default Firmware] (docs / default-firmware.md).
For more information about the JavaScript libraries, refer to [Controlling LINE Things Development Board via JavaScript] (docs / js-control.md).
LIFF app for the default firmware is at `liff-app / linethings-dev-default /` and `liff-app / linethings-js-control /`.

The firmware has a function to rewrite the Service UUID of the Advertising packet through LIFF.
The Service UUID can be obtained from the LINE Developers console and is used for building devices through LINE Things Developer Trial.
The function is to be used to allow devices to be unique without making modifications to the firmware.

##### Modify and initialize Service UUID from LIFF
LINE Things Developer Trial requires a product to be registered and a GATT Service UUID to be generated.
You can modify the Service UUID in the example LIFF app at (`liff-app / linethings-dev-default /`, `liff-app / linethings-js-control /`).
With this, you will not need to modify the device firmware.

###### Rewriting Service UUID from LIFF
Open the default LIFF app from LINE and head on over to the Write advertising packet tab.
Put the Service UUID you want to advertise in the text box and press the rewrite button.
If the display on the Dev board shows "BLE advertising uuid changed from LIFF ....", then the new Service UUID is now programmed onto the device.

Upon success, **Close LINE Things in LINE App and press the Reset button on the dev board**.

###### Return the Service UUID to its initial state
With the power on, press the reset button on the CPU board while pressing the SW1 tactile switch on the board.
You will see on the screen, `Set advertising packet UUID to default.` upon startup.

### Customizing the Firmware
[Adafruit open-source Arduino library] (https://github.com/adafruit/Adafruit_nRF52_Arduino) can be used to easily develop on Arduino IDE.

For hardware specifications, refer to [Hardware Specification] (# Hardware Specifications).
Refer to [Flashing the Firmware] (docs / update-firmware.md) for the steps on flashing the firmware.
The sample code, [Sample Code] (docs / examples.md), describes how to control each peripheral.

If you want to revert to the default firmware, use `arduino / linethings-dev-default / linethings-dev-default.ino`.
This example uses `things_temp_lib`,` SparkFun MMA8452Q Accelerometer`, `Adafruit SSD1306`,` Adafruit GFX Library`. `things_temp_lib` is available at ` library / things_temp_lib` compressed as a zip file. You will need to extract it into your Arduino library directory. For other libraries, please install them through the library manager.

[LINE Things Starter] (https://github.com/line/line-things-starter) is also compatible with LINE Things dev board.
From LINE Things Starter's repository, flash the firmware for the LINE Things dev board.

---

## How to use
### Inserting the CPU core and using the daughter board
First, ensure that the CPU core is installed onto the daughter board. To install the CPU core onto the daughter board, line up the pins of the CPU core to the daughter board. make sure that the Bluetooth antenna is pointing towards the **outside** of the board.

#### 1. Selecting the power source
The motherboard can be powered by external power sources such as through USB or a bench supply as well as on-board sources such as through batteries. Be sure to turn off the * SW3 * power switch before making these settings.
To power the board via external power sources such as USB or a bench supply, make sure the power connection **disconnected** first. Move the jumper on **P4** to the upper side to select external power source. Connect your power supply to power the board. 
Likewise, to use the internal power source, ie. battery, move the jumper on **P4** to the lower side to select the on-board power source. To turn the board on, you will need to slide SW3 to **On**.

[motherboard_power] (https://user-images.githubusercontent.com/135050/55808659-1d993400-7bfe-11e9-8bd1-298ff1bf8d40.jpg)

 * External power supply and USB power
   * Set *P4* to the top as seen from the front as shown in the picture.
 * Battery powered
   * Set *P4* to the bottom as seen from the front as shown in the picture.

※ 5V logic systems can not be used in case of battery power source. An external power source is advised
※ Do not use USB power to drive motors. An external power supply is advised to drive motors instead.

#### 2. Turning on the board

* External power supply and USB power
  * Connecting a USB cable or a power source through the *CN3* connector will turn on the board, regardless of the *SW3* switch's position.
* Battery powered
  * With the battery installed, slide the *SW3* power switch upwards to the ON position to turn the board on.

Make sure that the power supply LED (*DS1*) is on. You are now ready to use the development board.

### Using the CPU Core as a Standalone
The CPU core is fully functional as a standalone board, requiring only power to be supplied to it. The Reset pin is internally pulled up and can be left unconnected if unused.

## Usage Notes
### CPU board
[motherboard_direction] (https://user-images.githubusercontent.com/135050/58088649-16722600-7bfe-11e9-8b84-82e1e9ebd65e.jpg)

 * Before powering the development board on, be sure that the CPU core is connected in the correct orientation.
 * When using the CPU core as a standalone, ensure the antenna sits as far away from the breadboard as possible (preferably the edge of the breadboard) to avoid any interference. Additionally, any GND wiring and planes as well as any RF signals near the attena can cause interference.
 * For a reliable connection and communication, make sure to keep the antenna as exposed as possible. Avoid metallic enclosures as it can block BLE signals. Plastic enclosures are highly advised.

### Motherboard
 * Labels of **P2** and **P6** on the board are swapped. The correct labels should be, **P2** is I2C (5V) and **P6** is I2C (3.3V).
 * Labels of **P7** and **P8** on the board are swapped. The correct labels should be, **P7** is Grove-I/O and **P8** is Grove-UART.
 * If you are using batteries as your power supply, make sure the polarity of the batteries are properly placed. Reverse voltages may cause permanent damages to the boards.
 * If the board is powered externally at 3.3V, make sure to not draw power more than 600mA (total power draw, including the CPU core's consumption) to avoid any damage to the power regulator.
 * Ensure the polarity of the power source's connections as well as the voltage before making connections to the board.
 * Some parts may missing from the board, you may add them as needed.

---

## Hardware Specifications
### CPU core
[cpuboard] (https://user-images.githubusercontent.com/135050/58088657-1bcf7080-7bfe-11e9-89f4-5e6bbde1a363.jpg)

The [Raytac MDBT42Q] (http://www.raytac.com/product/ins.php?index_id=31) module is the main processor of this development board.
Adafruit's nRF52 Arduino library is compatible with this module and can be used for development through Arduino IDE. It also has JTAG pins available to be connected via J-LINK etc.
You can also develop everything from scratch using Nordic's nRF5 SDK. Both configurations of the board are supported; CPU core as a standalone as well as paired with the daughter board as a complete development board of LINE Things.

#### Pin assignment
[cpuboard_pinassign] (https://user-images.githubusercontent.com/135050/58088655-1a9e4380-7bfe-11e9-90d4-530f84e56242.png)

#### Technical Specification
The following is CPU core's specification.

| Item | Value |
---- | ----
Operational voltage | 1.7 to 3.6 V |
| Maximum RF Power | 4dBm |
| Flash size | 512KB |
| RAM size | 64KB |

### Daughter board
[daughterboard] (https://user-images.githubusercontent.com/135050/58088621-078b7380-7bfe-11e9-8d0d-8d9c216770b4.jpg)

Mounted on the daughter board are LEDs, tactile switches, temperature sensors, and an OLED screen.
**Please ensure the orientation of the CPU core when using the daughter board. Permanent damages may occur if placed incorrectly.**

#### Technical Specification
| Item | Value |
---- | ----
Power source | Battery / USB / external power supply |
Operational voltage (battery) | 3V (2 single AAA batteries 1.5V in series) |
Operational voltage (USB) | 5V |
Operational voltage (external supply) | 5V |
| Maximum continuous current consumption (battery powered / motor not used) | 0.6A |
| Maximum continuous current consumption (when using motor) | 2.6A |

## Peripherals on the daughter board
**NOTE 5V systems and 3.3V systems cannnot be directly connected to one another.**
The development board will act as a 5V system if powered by an external power source. Conversely, the development board will be a 3.3V system if powered by batteries. Please proceed with caution and make sure all connections are made with their corresponding voltages.
**You WILL damage your board if connections to an incompatible voltage system is made.**

### LED
The anode of the LED is connected to the microcontroller. Therefore, when the microcontroller outputs High, the LED will turn on. Please set the port to output when using it.

| Board number | Microcontroller pin number |
---- | ----
| DS2 | 7 |
| DS3 | 11 |
| DS4 | 19 |
| DS5 | 17 |

### Tactile switch
The tactile switches are not pulled up nor down on the circuit, therefore, you will need to set the pins to INPUT_PULLUP when programming the microcontroller.

| Board number | Microcontroller pin number |
---- | ----
| SW1 | 29 |
| SW2 | 28 |

### Buzzer
A piezoelectric buzzer is included. To make the buzzer sound, you will need to provide an AC signal from the microcontroller. Setting the pin to HIGH will not cause the buzzer to make a sound. Normally on a regular Arduino, you would be able to use the tone() function to create such signals, however the timer in the nRF52832 is incompatible with this library. Therefore, you will need to generate the signal yourself through timers, interrupts, etc. In the sample provided, we take advantage of FreeRTOS's timer to generate a 1KHz interrupt to toggle the pin state which creates a 500Hz signal to make the buzzer sound. If you want to change the pitch of the buzzer, you will need to play around with the timers. 
You can use the tone () function to produce any sound with regular Arduino, but you can not use this with the nRF52832 Arduino environment. 

| Board number | Microcontroller pin number |
---- | ----
| BZ1 | 27 |

### OLED
The board includes a 128x64 pixel OLED Screen (SSD1306), connected via I2C. You can control the board via Adafruit's SSD1306 Library. The I2C address of the screen is **0x3D**.


| Pin name | Microcontroller pin number |
---- | ----
| SCL | 26 |
| SDA | 25 |

### Temperature sensor
A temperature sensor (AT30TS74) is also included; connected via I2C, its address is at **0x48**.

| Device pin | Microcontroller pin number |
---- | ----
| SCL | 26 |
| SDA | 25 |

### Accelerometer
An accelerometer (MMA8452Q) is also connected via I2C at the address **0x1C**. Sparkfun provides a handy library for this module.

| Device pin | Microcontroller pin number |
---- | ----
| SCL | 26 |
| SDA | 25 |

### Motor
Two motor drivers (DRV8830) can be installed. Once soldered, they will be connected via I2C with the addresses at **0x60** (CN1) and **0x62** (CN2)

| Device pin | Microcontroller pin number |
---- | ----
| SCL | 26 |
| SDA | 25 |

#### Notes
* The motor driver is not installed by default. To use it, you need to remove some parts and solder additional parts yourself.
Please see [Motor Driver] (# Motor Driver) for details.
* Driving motors is for a 5V system. Please use an external power source to power the board when driving motors.

### Grove terminal (I2C)
The correct connection for this is **P6** on the board. There is a misprint on the silkscreen.

| Connector pin number | Pin name | Microcontroller pin number |
---- | ---- | ----
| 1 | SCL | 26 |
| 2 | SDA | 25 |
| 3 | 3.3V |-|
| 4 | GND |-|

#### Notes
* This connector is not installed by default. You need to solder the Grove connector to use it.
* Although the label on the board is Grove-I2C 5V on P6, the correct labels should be I2C-3.3V for P6 and I2C-5V for P2.

### Grove terminal (I2C / 5V system)
The correct connection for this is **P2** on the board. There is a misprint on the silkscreen.

| Connector pin number | Pin name | Microcontroller pin number |
---- | ---- | ----
| 1 | SCL | 26 |
| 2 | SDA | 25 |
| 3 | 5V |-|
| 4 | GND |-|

#### Notes
* This connector is not installed by default. You need to solder the Grove connector to use it.
* Although the label on the board is I2C-3.3V on P2, the correct labels should be I2C-3.3V for P6 and I2C-5V for P2.
* This is for 5V systems. Please make sure the board is powered by an external power source to use this port.
* Connect devices with 5V interfaces through this connector. Connecting a 3.3V device will cause damages to the board and the device.

### Grove terminal (UART)
The correct connection for this is **P8** on the board. There is a misprint on the silkscreen.

| Connector pin number | Pin name | Microcontroller pin number |
---- | ---- | ----
| 1 | RxD | 6 |
| 2 | TxD | 8 |
| 3 | 3.3V |-|
| 4 | GND |-|

#### Notes
* This connector is not installed by default. You need to solder the Grove connector yourself to use it.

### Grove terminal (general purpose digital I / O)
The correct connection for this is **P7** on the board. There is a misprint on the silkscreen.

| Connector pin number | Pin name | Microcontroller pin number |
---- | ---- | ----
| 1 | D0 | 2 |
| 2 | D 1 | 3 |
| 3 | 3.3V |-|
| 4 | GND |-|

#### Notes
* This connector is not installed by default. You need to solder the Grove connector yourself to use it.

### GPIO
The pins are at **P1** on the board.

Pin 6 is a reset pin. You can reset the microcontroller by setting it to LOW. It is pulled up on the CPU core board, so it can be disconnected if not used.

| Connector pin number | Pin name | Microcontroller pin number |
---- | ---- | ----
| 1 | I / O | 2 |
| 2 | 5V |-|
| 3 | I / O | 3 |
| 4 | 3.3V |-|
| 5 | I / O | 4 |
| 6 | RESET |-|
| 7 | I / O | 5 |
| 8 | I / O | 26 (SCL) |
| 9 | I / O | 12 |
| 10 | I / O | 25 (SDA) |
| 11 | I / O | 13 |
| 12 | I / O | 16 |
| 13 | I / O | 14 |
| 14 | I / O | 15 |
| 15 | I / O | 8 (RxD) |
| 16 | O | 11 (LED-DS3) |
| 17 | I / O | 6 (TxD) |
| 18 | O | 7 (LED-DS2) |
| 19 | GND | |
| 20 | GND | |

#### Notes
* Pins 8 and 10 are connnected via I2C to the CPU core since there are peripherals on-board using these pins. It is strongly recommended to use these pins as I2C pins instead of GPIO.
* Pins 15 and 17 are connected to the UART terminal which is also used for flashing firmware. It is strongly recommended to use these pins for UART connections instead of GPIO.
* Pins 16 and 18 are connected to LEDs on the board. It is strongly recommended to use these pins as output pins.

## Expansion
### Motor driver
Motor drivers are not pre-installed on the daughter board. In order to connect motors to the board, you will need to remove **R18** and solder the following parts on their labeled positions.

You can solder the DRV8830 DGQR GND pad by flowing solder from the reverse side of the board.


| Additional parts substrate silk | Parts | Parameters |
---- | ---- | ----
| U1 | IC | DRV8830DGQR |
| U2 | IC | DRV8830DGQR |
| L1 | Coil | 100uF / 2A |
| C1 | Capacitor | 1000pF |
| C2 | Capacitor | 1000pF |
| C13 | Capacitor | 100uF / 16V |
| C14 | Capacitor | 470uF / 16V |
| R10 | Resistor | 0.2Ω / 2W |
| R11 | Resistor | 0.2Ω / 2W |
| CN1 | Connector | JST-XH-02P |
| CN2 | Connector | JST-XH-02P |

### Power / Grove Connector
The power supply connector uses a JST-XH-02P connector. The Grove connector uses standard 4P connector.

### Battery Holder
The battery holder uses two AAA battery holders that can be purchased through Farnell/Element14 part number 1702632.

-https://www.newark.com/keystone/2466/battery-holder-1aaa-through-hole/dp/16F1095

## Schematics / Diagrams
-CPU core board
    -[Circuit Diagram] (https://github.com/line/lines-things-dev-board/blob/master/schematics/cpu_board/Outputs/schematic.pdf)
    -[Artwork Diagram-Table] (https://github.com/line/lines-things-dev-board/blob/master/schematics/cpu_board/Outputs/pcb_top.pdf)
    -[Artwork Diagram-Back] (https://github.com/line/lines-things-dev-board/blob/master/schematics/cpu_board/Outputs/pcb_bottom.pdf)
-Daughter board
    -[Circuit Diagram] (https://github.com/line/lines-things-dev-board/blob/master/schematics/main_board/Outputs/schematic.pdf)
    -[Artwork diagram-table] (https://github.com/line/lines-things-dev-board/blob/master/schematics/main_board/Outputs/pcb_top.pdf)
    -[Artwork Diagram-Back] (https://github.com/line/lines-things-dev-board/blob/master/schematics/main_board/Outputs/pcb_bottom.pdf)

### Ordering your own PCB
-[`/ cpu_board / Outputs /`] (/ cpu_board / Outputs /)
-[`/ main_board / Outputs /`] (/ main_board / Outputs /)

Gerber data required for board production is stored here. Use bom_{cpu/daughter}_board.xls for the parts list required for installation.
The circuit diagram and board are designed by Altium Circuit Studio. If you want to make modifications to the design, you can open it from Circuit Studio.
https://www.altium.com/circuitstudio/