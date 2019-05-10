

# Java Scriptからデバイスをより高度にコントロールする方法
デフォルトのファームウェアとLIFFではデバイスの状態を取得したりLEDの設定などができますが、ユーザが自由なタイミングでそれらを
操作することはできません。

ここで紹介する方法を用いると、ファームウェアの書き換えなしに、LIFFのJava Scriptによってデバイスの挙動を設定することが可能です。
例えば任意のタイミングでデバイスからNotifyさせたり、各種センサの値を任意に設定したり、取得することが可能です。

動作原理としては、デフォルトのLIFFが用いるUUIDとは別のキャラクタリスティックを用いてデバイスの操作を行っています。
なお、一度このキャラクタリスティックを用いると、デバイスのOLEDの表示は更新されなくなります。
サイド表示させたい場合はデバイスの電源を入れ直すかリセットボタンを押してください。

default/に格納されているコードではここで紹介している方法でコントロールできる全ての機能を試すことができます。まずはこちらを試してみると雰囲気をつかめるかもしれません。

## 利用するにあたって
この機能を利用する場合はFirmwareのバージョンが2以上でなくてはなりません。
LIFFを開いた際、アラートが表示された場合はファームウェアのアップデートをしてください。

詳しくは[ファームウェアの書き込み方法](https://github.com/line/line-things-dev-board#%E3%83%95%E3%82%A1%E3%83%BC%E3%83%A0%E3%82%A6%E3%82%A7%E3%82%A2%E6%9B%B8%E3%81%8D%E8%BE%BC%E3%81%BF%E6%96%B9%E6%B3%95)
を御覧ください。


# 利用方法
この新しい方法は `js-framework.js` に実装されています。使用する際にはhtmlからこのファイルを読み込んでください。

利用するには、LINE EngineeringにてLIFFのendpointを作成したLIFFに設定する必要があります。LIFFを開いたとき初めてそのLIFFが実行されます。

いくつかサンプルを用意しています。初めて利用する際はそれを参考にしたり、拡張すると便利です。

|ファイル名|内容|
----|----
|default|利用できるすべての機能を試してみることができます|
|display|LIFFからデバイスのOLEDを制御するサンプルです|
|led-blink|LIFFからLEDを制御します。Arduinoライクにsetup(), loop()を利用できるようにしています|
|notify|SWとTemperatureの値を定期的にnotifyするサンプルです|

led-blinkはloopコードを含んでいます。あくまでArduinoライクに動かすためにそうしていますが、端末の電力を消費します。本来はイベントドリブンで書くべきなので、あくまでも
動かせるデモとしてのみ参考にしてみてください。



# 機能詳細

## Service UUIDの書き換え
`writeAdvertUuid(uuid)`
uuidにはテキストとして書き換えたいUUIDを設定します。なお、このメソッドを実行しても即座には反映されず、デバイスのリセットとLIFFの再立ち上げが
必要です。

この機能のみ、Version 1から利用可能です。

## Versionの読み込み
`deviceVersionRead()`
このメソッドを実行したあと、 `versionRead()` にてファームウェアのバージョンを取得します。
なお、以前のバージョンのファームウェアはバージョン情報のキャラクタリスティックが存在しないため、
存在しないキャラクタリスティックとして検出し、その場合はバージョン1を返します。

これから説明している機能はVersion 2以上で利用可能となります。


## Display 制御
### ディスプレイクリア
`displayClear()`
ディスプレイに表示されているテキストを消します。

### ディスプレイ書き込みポジション指定
`displayControl(addr_x, addr_y)`
書き込むアドレスを指定します。それぞれ0~63の値を取ります。

### キャラクタフォントサイズ指定
`displayFontSize(size)`
フォントサイズを指定します。指定可能な値は1, 2, 3です。

### キャラクタ書き込み
`displayWrite(text)`
textには書き込みたい文字列(最大16文字)を指定します。16文字を消える場合は複数回に分けて実行します。

## LED制御
### 単体制御
`ledWrite(port, value)`
portは対象のLEDを指定します。2~5を指定可能です。それぞれデバイスのLED DS2~5が対応しています。valueは0で消灯、1で点灯です。

### 全体制御
`ledWriteByte(value)`
全てのLEDを一度の制御します。


## ブザー制御
`buzzerControl(value)`
1を書き込むとbuzzerがなります。

## GPIOデジタル制御
### 入出力設定
`gpioPinMode(port, value)`
portは対象のGIPOポートを指定します。valueは0でINPUT, 1でOUTPUTです。

### 出力
`gpioDigitalWrite(port, value)`
portは対象のGIPOポートを指定します。valueは対象ポートの出力レベルを0または1で設定します。

## GPIOアナログ出力
`gpioAnalogWrite(port, value)`
portは対象のGIPOポートを指定します。valueは対象ポートの出力レベルを0から255で指定します。

## I2C
`i2cStartTransmission(address)`
`i2cWrite(value)`
`i2cStopTransmission()`
`i2cRequestFrom(address, length)`
`i2cReadRequest(device)`

基本的にはArduinoのI2Cと同じように使用するので、そちらを参考にしてください。 `i2cReadRequest(device)` は
I2Cデバイスからデータを読み込み、Dev Board内のバッファに格納します。実際のそのデータをLIFFに転送するには
 `readReq(cmd)` と `deviceRead()` を組み合わせて行います。


## デバイスからのデータリード
デバイスからのデータリードは2段階で行われます。まず、デバイス内で読み込みたいデータをそれぞれのバッファに格納します。
deviceおよびportで対象となるデバイスのアドレスや、読み込むポート番号を指定します。なお、スイッチ、加速度センサ、
温度に関してはこのステップは不要で、いきなり `readReq(cmd)` を実行します。

`i2cReadRequest(device)`
`gpioDigitalReadReq(port)`
`gpioAnalogReadReq(port)`


読み込みたいデータをバッファに格納したら、実際に読み込みたいソースを指定します。
`readReq(cmd)`

|値|説明|
----|----
|0|Switch|
|1|加速度センサ|
|2|温度|
|3|デジタルGPIOの値|
|4|アナログGPIOの値|
|5|I2Cのデータ|

最後に、`deviceRead()`を実行して、デバイスのバッファに格納されたデータをLIFF上に転送します。転送したデータは`valueRead()`で得ることができます。


## Notification
### Switch
`swNotifyEnable(source, mode, interval, callback)`
スイッチの状態に変化が起きたときだけNotifyします。`swNotifyDisable()`で停止します。
#### source

|値|説明|
----|----
|0|disable|
|1|SW1|
|2|SW2|
|3|SW1 or SW2|

#### mode

|値|説明|
----|----
|0|LOW|
|1|CHANGE|
|2|RISING|
|3|FALLING|

#### interval
一度割り込みを起こしてから次に割り込みを許可するまでの時間をミリ秒で指定します。これを使用することでチャタリング防止機能をつけることが可能です。

#### callback
Notificationが発生したときに実行されるメソッドを指定します。

### Temperature
`tempNotifyEnable(interval, callback)`
温度を定期的にNotifyします。`tempNotifyDisable()`で停止します。
なお、 `i2cStartTransmission(address)`を実行したあと、この機能は `i2cStopTransmission()` が実行されるまで無視されます。

#### source

|値|説明|
----|----
|0|disable|
|1|enable|


#### callback
Notificationが発生したときに実行されるメソッドを指定します。
