# Arduino で利用できるサンプルコード

Arduino で、LINE Things development board 搭載デバイスを動作させるためのサンプルコードです。
ファームウェアの書き込み方法は、[ファームウェア書き込み方法](update-firmware.md) を参考にしてください。

## サンプルコード - LED
```cpp
/**
 * サンプルコード - LED
 * 0.5秒ごとにDS2~5が点灯してすべて点灯したら0.5秒に消灯を繰り返す
 */

const int LED_DS2 = 7;
const int LED_DS3 = 11;
const int LED_DS4 = 19;
const int LED_DS5 = 17;

void setup() {
  // LEDのピンを出力に設定
  pinMode(LED_DS2, OUTPUT);
  pinMode(LED_DS3, OUTPUT);
  pinMode(LED_DS4, OUTPUT);
  pinMode(LED_DS5, OUTPUT);
}

void loop() {
  // 500mSごとにLED_DS2~5を点灯
  digitalWrite(LED_DS2, HIGH);  // LED_DS2を点灯
  delay(500);                   // 500mS待つ
  digitalWrite(LED_DS3, HIGH);
  delay(500);
  digitalWrite(LED_DS4, HIGH);
  delay(500);
  digitalWrite(LED_DS5, HIGH);
  delay(500);
  // LED_DS2~5を消灯
  digitalWrite(LED_DS2, LOW);  // LED_DS2を消灯
  digitalWrite(LED_DS3, LOW);
  digitalWrite(LED_DS4, LOW);
  digitalWrite(LED_DS5, LOW);
  delay(500);
}
```


## サンプルコード - タクトスイッチ
```cpp
/**
 * サンプルコード - タクトスイッチ
 * SW1が押されているときだけLED_DS2が点灯
 */

const int LED_DS2 = 7;
const int SW1 = 29;

void setup() {
  // スイッチをプルアップ付き入力に設定
  pinMode(SW1, INPUT_PULLUP);
  // LEDのピンを出力に設定
  pinMode(LED_DS2, OUTPUT);
}

void loop() {
  // SW1が押されているときだけLED_DS2が点灯
  digitalWrite(LED_DS2, !digitalRead(SW1));
}
```

## サンプルコード - ブザー
```cpp
/**
 * サンプルコード - ブザー
 * 0.5秒ごとにブザーがON/OFFします
 */

const int BUZZER_PIN = 27;
SoftwareTimer buzzer;

// ブザーを鳴らすために1KHzの周期でイベントを生成
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

## サンプルコード - OLED
```cpp
/**
 * サンプルコード - OLED
 * `Adafruit SSD1306` と `Adafruit GFX Library` ライブラリをインストールしてください
 */

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

// ディスプレイのインスタンスを生成
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  // ディスプレイの初期化
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // ディスプレイの表示に必要な電圧を生成, アドレスは0x3C
  display.clearDisplay();                       // ディスプレイのバッファを初期化
  display.display();                            // ディスプレイのバッファを表示
}

void loop() {
  // テキストを描画
  display.setTextSize(1);                       // テキストサイズ1
  display.setTextColor(WHITE);                  // Color White
  display.setCursor(0,10);                      // X=0, Y=10
  display.println("LINE Things");
  display.println("Starter Board");
  display.display();                            // ディスプレイのバッファを表示
  delay(1000);
  // 線を描画
  display.drawLine(0, 32, 128, 32, WHITE);      
  display.display();
  delay(1000);
  // 四角を描画
  display.fillRect(20, 40, 20, 20, WHITE);
  display.display();
  delay(1000);
  // 丸を描画
  display.fillCircle(100, 50, 10, WHITE);
  display.display();
  delay(1000);
  // 表示をクリア
  display.clearDisplay();                       // ディスプレイのバッファを初期化
  display.display();                            // ディスプレイのバッファを表示
  delay(100);
}
```

## サンプルコード - 温度センサ
```cpp
/**
 * サンプルコード - 温度センサ
 * 温度を読み込んでOLEDに表示するサンプルです。温度の表示にOLEDを使っています。
 * `library/things_temp_lib.zip` をライブラリをインクルードからインストールしてください
 * `Adafruit SSD1306` と `Adafruit GFX Library` ライブラリを管理からインストールしてください
 */

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <linethings_temp.h>

//ディスプレイのインスタンスを生成
Adafruit_SSD1306 display(128, 64, &Wire, -1);

ThingsTemp temp = ThingsTemp();

void setup() {
  // ディスプレイの初期化
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // ディスプレイの表示に必要な電圧を生成, アドレスは0x3C
  display.clearDisplay();                       // ディスプレイのバッファを初期化
  display.display();                            // ディスプレイのバッファを表示

  // 温度センサの初期化
  temp.init();
}

void loop() {
  // 温度を取得
  float data = temp.read();
  // 温度を表示
  display.clearDisplay();                       // ディスプレイのバッファを初期化
  display.setTextSize(1);                       // テキストサイズ1
  display.setTextColor(WHITE);                  // Color White
  display.setCursor(0,10);                      // X=0, Y=10
  display.println("LINE Things");
  display.println("Starter Board");
  display.print("Temperature:");
  display.print(data);
  display.println("C");
  display.display();                            // ディスプレイのバッファを表示
  delay(1000);
}
```

## サンプルコード - 加速度センサ
```cpp
/**
 * サンプルコード - 加速度センサ
 * 加速度センサの値を読み込んでOLEDに表示するサンプルです。表示にOLEDを使っています。
 * `SparkFun MMA8452Q Accelerometer` ライブラリを管理からインストールしてください
 * `Adafruit SSD1306` と `Adafruit GFX Library` ライブラリを管理からインストールしてください
 */

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SparkFun_MMA8452Q.h>

// ディスプレイのインスタンスを生成
Adafruit_SSD1306 display(128, 64, &Wire, -1);
// 加速度センサのインスタンスを生成 (アドレス: 0x1c)
MMA8452Q accel(0x1c);

void setup() {
  // ディスプレイの初期化
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // ディスプレイの表示に必要な電圧を生成, アドレスは0x3C
  display.clearDisplay();                       // ディスプレイのバッファを初期化
  display.display();                            // ディスプレイのバッファを表示

  // 加速度センサの初期化
  accel.init(SCALE_2G);
}

void loop() {
  // 加速度センサから値を取得
  if (accel.available()) {
    accel.read();
  }

  // 加速度センサの値を表示
  display.clearDisplay();                       // ディスプレイのバッファを初期化
  display.setTextSize(1);                       // テキストサイズ1
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
  display.display();                            // ディスプレイのバッファを表示
  delay(300);
}
```

## サンプルコード - モーター
```cpp
/**
 * サンプルコード - モーター
 * 2つのモーターを制御するサンプルです
 * library/things_motor_lib.zipをライブラリをインクルードからインストールしてください
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

## マザーボード上以外のデバイスを使ったサンプルスケッチ

### フルカラー LED - NeoPixel
![Neopixel](../img/neopixel.jpg)

一般的にNeopixelとよばれる、WS2812B搭載のフルカラーLEDを光らせるデモです。LEDの光の輝度を時間とともに0~100%でコントロールしています。マザーボード上の *SW1* を押すことでLEDの色を変えることができます。
GPIOの1ピン(マイコン *IO2*)にNeopixelのIN端子を、GPIOの3番ピン(3.3V)を *VCC*、20番ピン(GND)を *GND* に接続します。本来WS2812Bは5Vで使用するものですが、3.3V電源でも一応動くことを確認しています。

ファームウェアは `arduino/linethings-dev-neopixel/linethings-dev-neopixel.ino` を使用してください。
使用する基板に搭載されたLED数に従って `NEOPIXEL_PIXELS` の値を変更してください。初期値は16個のLEDを制御します。

Adafruit_NeoPixelライブラリを使用しています。ライブラリを管理から `adafruit neopixel` と検索して見つかる `Adafruit Neopixel by Adafruit` をインストールしてください。

### RC car
マザーボード上に搭載されたモータードライバを使ったラジコンです。LINE Thingsを使ってコントロールすることが可能です。
ファームウェアは `arduino/linethings-dev-car/linethings-dev-car.ino` を使用してください。LIFFは `liff-app/linethings-dev-car/` においてあるものを使用します。

### 注意事項
* マザーボード基板上の電池のみでは動作しません。外部電源コネクタを使用して給電します。
* USB電源はパソコンなどから給電すると電圧が不安定になり動作しない場合があります。また流れる電流に注意が必要なのでおすすめしません。
