# Sample code for Arduino

These are sample codes for the each peripheral on LINE Things developement board. Please refer to [Flashing Firmware](update-firmware.md) for [Flashing the Firmware](update-firmware.md).

## LED Example
```cpp
/**
* LED Example
* Lights DS2 ~ DS5 will light up following one another every 0.5s. Afer all lights are lit, they will all turn off after 0.5s. The process will repoeat indefinitely.
*/

const int LED_DS2 = 7;
const int LED_DS3 = 11;
const int LED_DS4 = 19;
const int LED_DS5 = 17;

void setup() {
  // We set each LED pin to OUTPUT
  pinMode(LED_DS2, OUTPUT);
  pinMode(LED_DS3, OUTPUT);
  pinMode(LED_DS4, OUTPUT);
  pinMode(LED_DS5, OUTPUT);
}

void loop() {
  // We light up each LED every 500ms
  digitalWrite(LED_DS2, HIGH);  // LED_DS2 is turned on
  delay(500);                   // 500ms wait
  digitalWrite(LED_DS3, HIGH);
  delay(500);
  digitalWrite(LED_DS4, HIGH);
  delay(500);
  digitalWrite(LED_DS5, HIGH);
  delay(500);
  // Turning off LED_DS2~5
  digitalWrite(LED_DS2, LOW);  // LED_DS2 is turned off
  digitalWrite(LED_DS3, LOW);
  digitalWrite(LED_DS4, LOW);
  digitalWrite(LED_DS5, LOW);
  delay(500);
}
```


## Tactile Switch Example
```cpp
/**
* Tactile Switch Example
* LED LED_DS2 is lit when SW1 is pressed
*/

const int LED_DS2 = 7;
const int SW1 = 29;

void setup() {
  // We set the switch to INPUT with internal pull-up
  pinMode(SW1, INPUT_PULLUP);
  // We set the LED pin to OUTPUT
  pinMode(LED_DS2, OUTPUT);
}

void loop() {
  // LED LED_DS2 is lit when SW1 is pressed
  digitalWrite(LED_DS2, !digitalRead(SW1));
}
```

## Buzzer Example
```cpp
/**
 * Buzzer Example
 * The buzzer will ring every 0.5s
 */

const int BUZZER_PIN = 27;
SoftwareTimer buzzer;

// Generate a signal with a 1KHz cycle
void buzzerEvent(TimerHandle_t xTimerID) {
  digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
}

void buzzerStart() {
  pinMode(BUZZER_PIN, OUTPUT);
  buzzer.begin(1, buzzerEvent);
	buzzer.start();
}

void buzzerStop() {
  buzzer.stop();
  digitalWrite(BUZZER_PIN, 0);
}

void setup() {
}

void loop() {
  buzzerStart();
  delay(500);
  buzzerStop();
  delay(500);
}
```

## OLED Example
```cpp
/**
* OLED Example
* Make sure to install the `Adafruit SSD1306 Library` and `Adafruit GFX Library`
*/

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

// Creating an instance of the display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  // Initialization of the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // Generate the voltage for the display as well as identify the address
  display.clearDisplay();                       // Clear the memory buffer of the display
  display.display();                            // Display the buffer
}

void loop() {
  // Display Text
  display.setTextSize(1);                       // Setting Text size to 1
  display.setTextColor(WHITE);                  // Color White
  display.setCursor(0,10);                      // X=0, Y=10
  display.println("LINE Things");
  display.println("Starter Board");
  display.display();                            // Display the buffer
  delay(1000);
  // Drawing a line
  display.drawLine(0, 32, 128, 32, WHITE);      
  display.display();
  delay(1000);
  // Drawing a rectangle
  display.fillRect(20, 40, 20, 20, WHITE);
  display.display();
  delay(1000);
  // Drawing a circle
  display.fillCircle(100, 50, 10, WHITE);
  display.display();
  delay(1000);
  // clearing the display
  display.clearDisplay();                       // Clear the memory buffer
  display.display();                            // Display the buffer
  delay(100);
}
```

## Temperature Sensor Example
```cpp
/**
* Temperature Sensor Example
* This example takes advantage of the OLED to display the temperature reading.
* Make sure to have the included library `library/things_temp_lib.zip` installed in your library directory
* Make sure to install the `Adafruit SSD1306 Library` and `Adafruit GFX Library`
*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <linethings_temp.h>

// Creating an instance of the display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

ThingsTemp temp = ThingsTemp();

void setup() {
  // Initialization of the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // Generate the voltage for the display as well as identify the address
  display.clearDisplay();                       // Clear the memory buffer of the display
  display.display();                            // Display the buffer

  // Initialize the temperature sensor
  temp.init();
}

void loop() {
  // Get the temperature reading
  float data = temp.read();
  // Displaying the temperature
  display.clearDisplay();                       // Clear the memory buffer
  display.setTextSize(1);                       // Set Text Size to 1
  display.setTextColor(WHITE);                  // Color White
  display.setCursor(0,10);                      // X=0, Y=10
  display.println("LINE Things");
  display.println("Starter Board");
  display.print("Temperature:");
  display.print(data);
  display.println("C");
  display.display();                            // Display the buffer
  delay(1000);
}
```

## Accelerometer Example
```cpp
/**
* Accelerometer Example
* This example takes advantage of the OLED to display the accelerometer reading.
* Make sure to install the `SparkFun MMA8452Q Accelerometer Library`
* Make sure to install the `Adafruit SSD1306 Library` and `Adafruit GFX Library`
*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SparkFun_MMA8452Q.h>

// Creating an instance of the display
Adafruit_SSD1306 display(128, 64, &Wire, -1);
// Creating an instance of the accelerometer at address 0x1c
MMA8452Q accel(0x1c);

void setup() {
  // Initialization of the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // Generate the voltage for the display as well as identify the address
  display.clearDisplay();                       // Clear the memory buffer of the display
  display.display();                            // Display the buffer

  // Initializing the accelerometer
  accel.init(SCALE_2G);
}

void loop() {
  // Get value from accelerometer
  if (accel.available()) {
    accel.read();
  }

  // Displaying the values
  display.clearDisplay();                       // Clear the memory buffer
  display.setTextSize(1);                       // Set Text Size to 1
  display.setTextColor(WHITE);                  // Color White
  display.setCursor(0,10);                      // X=0, Y=10
  display.println("LINE Things");
  display.println("Starter Board");
  display.print("X:");
  display.println(accel.cx);
  display.print("Y:");
  display.println(accel.cy);
  display.print("Z:");
  display.println(accel.cz);
  display.display();                            // Display the Buffer
  delay(300);
}
```

## Motor Example
```cpp
/**
 * Motor Example
 * This exmample will control 2 motors
 * Please install the included library/things_motor_lib.zip library
 */

#include <linethings_motor.h>

ThingsMotor motor_cn1 = ThingsMotor(MOTOR_ADDR_CN1);
ThingsMotor motor_cn2 = ThingsMotor(MOTOR_ADDR_CN2);

int i = 0;

void setup() {
  motor_cn1.init();
  motor_cn2.init();
}

void loop() {
  motor_cn1.control(MOTOR_BACK, i);
  motor_cn2.control(MOTOR_FORWARD, i);
  delay(100);
  i++;
  if (i >= 100) {
    i = 0;
  }
}
```

## Sample Sketches with peripherals not on the daughter board

### Addressable RGB LED - NeoPixel
![neopixel](https://user-images.githubusercontent.com/135050/58088640-12de9f00-7bfe-11e9-8a8e-6c73594a59f3.jpg)

This example lights up addressable LEDs (WS2812B), also known as NeoPixels. The brightness of the LEDs will progressively get brighter, 0-100%. You can change the colors of the LEDs bu pressing **SW1** on the daughter board. GPIO 1 (or IO2 on the CPU core) is connected to the **Data IN** port of the NeoPixel, GPIO 3 (3.3V) is connected to **VCC**, and GPIO 20 (GND) is connected to **GND**. Although the NeoPixels are made to work at 5V, it has been tested and proven to work at 3.3V.

The sketch is available at `arduino/linethings-dev-neopixel/linethings-dev-neopixel.ino`.
You will need to change the number of pixels on your strip at `NEOPIXEL_PIXELS`. The default length is 16 LEDs.

Adafruit provides a hand library called **Adafruit_NeoPixel**. Search for `adafruit neopixel` in the library manager and install `Adafruit Neopixel by Adafruit` to use the NeoPixels.

### RC car
This will allow you to make an RC Car with the development board to be controlled via LINE.
Using the sketch `arduino/linethings-dev-car/linethings-dev-car.ino` and the LIFF application `liff-app/linethings-dev-car/` you can make your very own LINE controlled remote car.
#### Precautions
* Batteries as a power source will not work for this sample, please resort to using an external power source.
* USB Power from PC or handheld devices may provide unstable power due to the high power consumption of the motors.