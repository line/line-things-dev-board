# JavaScript から LINE Things Board を制御する

LINE Things developemt boardのファームウェアが持っているGATT Characteristicを利用することで、
自由にボードに乗っているハードウェアや GPIO を簡単に制御することができるようになっています。

ここで紹介する方法を用いると、ファームウェアの書き換えなしに、LIFFのJavaScriptによってデバイスの挙動を設定することが可能です。
制御を簡単に行うための、JavaScriptクラスを用意しています。
例えば任意のタイミングでデバイスからNotifyさせたり、各種センサの値を任意に設定したり、取得することが可能です。
LIFFからだけではなく、自動通信など、キャラクタリスティックに読み書きができる環境であれば、同じようにデバイスの操作をすることが可能です。

なお、このキャラクタリスティックからディスプレイ操作をすると、デバイスのOLEDの表示は更新されなくなります。
再度自動的に表示させたい場合はデバイスの電源を入れ直すかリセットボタンを押してください。

`liff-app/linethings-dev-js-control` に格納されているコードではここで紹介している方法でコントロールできる全ての機能を試すことができます。
まずはこちらを試してみると雰囲気をつかめるかもしれません。

その他サンプルコードを、`liff-app/examples-js-control` に格納しているので、こちらを参考にしてみてください。

## 利用するにあたって
この機能を利用する場合はLINE Things development boardのFirmwareバージョンが2以上でなくてはなりません。
LIFFを開いた際、アラートが表示された場合はファームウェアのアップデートをしてください。

詳しくは[ファームウェアの書き込み方法](https://github.com/line/line-things-dev-board#%E3%83%95%E3%82%A1%E3%83%BC%E3%83%A0%E3%82%A6%E3%82%A7%E3%82%A2%E6%9B%B8%E3%81%8D%E8%BE%BC%E3%81%BF%E6%96%B9%E6%B3%95)
を御覧ください。

## LIFF からの利用方法
JavaScriptから利用するためのクラスは`things-dev-board.js` に実装されています。
使用する際にはhtmlからこのファイルを読み込んでください。
LIFF の BLE プラグインに対して、`requestDevice()` した際に帰ってくる返り値をコンストラクタに代入して `connect()` してください。

利用するには、LINE EngineeringにてLIFFのendpointを作成したLIFFに設定する必要があります。
いくつかサンプルを用意しています。初めて利用する際はそれを参考にしたり、拡張すると便利です。
(GitHub pages も利用可能です)

|ファイル名|内容|
----|----
|default|利用できるすべての機能を試してみることができます|
|display|LIFFからデバイスのOLEDを制御するサンプルです|
|led-blink|LIFFからLEDを制御します。Arduinoライクにsetup(), loop()を利用できるようにしています|
|notify|SWとTemperatureの値を定期的にnotifyするサンプルです|

led-blinkはloopコードを含んでいます。あくまでArduinoライクに動かすためにそうしていますが、端末の電力を消費します。
本来はイベントドリブンで書くべきなので、あくまでも動かせるデモとしてのみ参考にしてみてください。

## JavaScriptライブラリ機能詳細

### Service UUIDの書き換え
`writeAdvertUuid(uuid)`
uuidにはテキストとして書き換えたいUUIDを設定します。なお、このメソッドを実行しても即座には反映さ
れず、デバイスのリセットとLIFFの再立ち上げが必要です。

この機能のみ、Version 1から利用可能です。

一度書き換えたUUIDはデバイスのFlashに格納されて、電源を切ったとしても最後に書き込んだService UUIDが使用されます。
デフォルトのService UUIDに戻したい場合、SW1スイッチを押しながらリセットボタンを押してください。

### Versionの読み込み
`deviceVersionRead()`
このメソッドを実行したあと、 `versionRead()` にてファームウェアのバージョンを取得します。
なお、以前のバージョンのファームウェアはバージョン情報のキャラクタリスティックが存在しないため、
存在しないキャラクタリスティックとして検出し、その場合はバージョン1を返します。

これから説明している機能はVersion 2以上で利用可能となります。

### Display 制御
この操作を行うと、ボード上でディスプレイの更新は行われなくなり、完全にこれから紹介する方法で
ディスプレイの表示内容を操作することになります。
もとに戻したい場合は、デバイスの電源を入れ直すかリセットボタンを押します。

#### ディスプレイクリア
`displayClear()`
ディスプレイ表示をクリアします。

#### ディスプレイ書き込みポジション指定
`displayControl(addr_x, addr_y)`
テキストを書き込むアドレスを指定します。それぞれ0~63の値を取ります。

#### キャラクタフォントサイズ指定
`displayFontSize(size)`
フォントサイズを指定します。指定可能な値は1, 2, 3です。

#### キャラクタ書き込み
`displayWrite(text)`
textには書き込みたい文字列(最大16文字)を指定します。16文字を消える場合は複数回に分けて実行します。

### LED制御
#### 単体制御
`ledWrite(port, value)`
portは対象のLEDを指定します。2~5を指定可能です。それぞれデバイスのLED DS2~5が対応しています。valueは0で消灯、1で点灯です。

#### 全体制御
`ledWriteByte(value)`
全てのLEDを一度の制御します。

### ブザー制御
`buzzerControl(value)`
1を書き込むとbuzzerがなります。

### GPIOデジタル制御
#### 入出力設定
`gpioPinMode(port, value)`
portは対象のGIPOポートを指定します。valueは0でINPUT, 1でOUTPUTです。

#### 出力
`gpioDigitalWrite(port, value)`
portは対象のGIPOポートを指定します。valueは対象ポートの出力レベルを0または1で設定します。

### GPIOアナログ出力
`gpioAnalogWrite(port, value)`
portは対象のGIPOポートを指定します。valueは対象ポートの出力レベルを0から255で指定します。

### I2C
`i2cStartTransmission(address)`
`i2cWrite(value)`
`i2cStopTransmission()`
`i2cRequestFrom(address, length)`
`i2cReadRequest(device)`

基本的にはArduinoのI2Cと同じように使用するので、そちらを参考にしてください。 `i2cReadRequest(device)` は
I2Cデバイスからデータを読み込み、Dev Board内のバッファに格納します。実際のそのデータをLIFFに転送するには
 `readReq(cmd)` と `deviceRead()` を組み合わせて行います。


### デバイスからのデータリード
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

最後に、`deviceRead()`を実行して、デバイスのバッファに格納されたデータを読み取ります。


### Notification
#### Switch
`swNotifyEnable(source, mode, interval, callback)`
スイッチの状態に変化が起きたときだけNotifyします。`swNotifyDisable()`で停止します。
##### source

|値|説明|
----|----
|0|disable|
|1|SW1|
|2|SW2|
|3|SW1 or SW2|

##### mode

|値|説明|
----|----
|0|LOW|
|1|CHANGE|
|2|RISING|
|3|FALLING|

##### interval
一度割り込みを起こしてから次に割り込みを許可するまでの時間をミリ秒で指定します。これを使用することでチャタリング防止機能をつけることが可能です。

##### callback
Notificationが発生したときに実行されるメソッドを指定します。

#### Temperature
`tempNotifyEnable(interval, callback)`
温度を定期的にNotifyします。`tempNotifyDisable()`で停止します。
なお、 `i2cStartTransmission(address)`を実行したあと、この機能は `i2cStopTransmission()` が実行されるまで無視されます。

##### source

|値|説明|
----|----
|0|disable|
|1|enable|

##### callback
Notificationが発生したときに実行されるメソッドを指定します。

