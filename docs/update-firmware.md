# ファームウェア書き込み方法
ここではArduino IDEを使用する方法を説明します。

## Arduino環境とドライバの準備
Arduino IDE をインストールた後、必ずボードを **接続しないで** 以下の手順を進めてください。

1. Arduino IDE を開きます
2. **Preferences** を開いてください
3. 'Additional Board Manager URL' に `https://www.adafruit.com/package_adafruit_index.json` を追加します
4. Tools -> Board menu　から **Boards Manager** を開いてください
5. "nRF52" と検索して、**Adafruit nRF52 by Adafruit** をインストールしてください (Version 0.11.0 以上をインストールしてください)
(*Note: Linux をお使いの方は追加でソフトウェアの設定が必要です、[こちらをご覧ください](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/arduino-bsp-setup)*)
6. [CP2102N driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) をインストールします

## ハードウェアセットアップ
### マザーボードにCPU基板を刺して使用する
1. CPUボードがマザーボードに刺さっていることを確認してください
2. USB端子(J1)にMicro-USBケーブルを使ってPCへ接続してください
3. Tools -> Board のリストから **Adafruit Bluefruit Feather nRF52832** を選択してください
4. Under Tools -> Port で正しいシリアルポートを選択してください *ie. COM1, /dev/cu.SLAB_USBtoUART*
5. テストとして、一度空のスケッチをアップロードしてみましょう。エラーがでなければ問題ありません。

### CPU基板単体で使用する - マザーボードの書き込み機を利用
1. 下記図のようにCPUボードの *RxD*/*TxD*/*Reset*/*VCC*/*GND* ピンをジャンパ線などを使用してマザーボードに接続してください。ピンアサインはCPUボードのピンアサインを参照してください。
2. USB端子(*J1*)にMicro-USBケーブルを使ってPCへ接続してください
3. Tools -> Board のリストから **Adafruit Bluefruit Feather nRF52832** を選択してください
4. Under Tools -> Port で正しいシリアルポートを選択してください *ie. COM1, /dev/cu.SLAB_USBtoUART*
5. テストとして、一度空のスケッチをアップロードしてみましょう。エラーがでなければ問題ありません。

![motherboard_external](https://user-images.githubusercontent.com/135050/58088646-14a86280-7bfe-11e9-974d-d4925148f3ae.jpg)

### CPU基板単体で使用する - 市販されている書き込み機を使用(非推奨)
DTR信号を出力できるアダプタであれば市販されているUSB-TTL変換アダプタ (CP2102搭載基板等) を使って書き込むことができます。この他に0.1uFのセラミックコンデンサが必要です。

1. 下記図のようにCPU基板の *RxD*/*TxD*/*Reset*/*VCC*/*GND* ピンをジャンパ線などを使用して書き込み機に接続してください(**ResetピンとDTRピンの間には0.1uFのセラミックコンデンサを挟んでください**)
2. USB端子(*J1*)にMicro-USBケーブルを使ってPCへ接続してください
3. Tools -> Board のリストから **Adafruit Bluefruit nRF52 Feather** を選択してください
4. Under Tools -> Port で正しいシリアルポートを選択してください *ie. COM1, /dev/cu.SLAB_USBtoUART*
5. テストとして、一度空のスケッチをアップロードしてみましょう。エラーがでなければ問題ありません。

| 書き込み器側 | CPU基板側 | 注意点 |
----|----|----
| VDD(3.3V) | P28 | |
| GND | P1 | |
| DTR | P27 | 間に0.1uFのコンデンサを挟んでください |
| RxD | P6 | |
| TxD | P8 | |

![cpuboard_prog_external](https://user-images.githubusercontent.com/135050/58088653-18d48000-7bfe-11e9-9b5a-f9637b23e590.jpg)

## 詳しい人向け
### J-LINKでの書き込み
J-LINKを使うとより高レベルのデバッグや、Arduinoを使用しない開発が可能となります。写真で示すJ-LINKコネクタの通りにデバッガを接続してください。
なお、J-LINKを使用して書き込みを行うと、もとのブートローダが破壊され、Arduino環境でのIDEが使用できなくなります。
さらに、工事設計認証(技適)を無効化してしまう可能性があります。よくわかる方のみ、ご自身の責任で行ってください。再度Arduino環境で使用できるように戻したい場合、
[Bootloaderを書き込む](#bootloaderを書き込む) を参考にして行ってください。

#### 配線図
![cpuboard_jlink](https://user-images.githubusercontent.com/135050/58088636-1114db80-7bfe-11e9-8a03-2222d94d49c5.png)

| コネクタピン番号 | ピン名称 |
----|----|
| 1 | SWDIO |
| 2 | GND |
| 3 | SWDCLK |
| 4 | VCC |


### AdafruitのBootloaderを書き込む
一度、J-LINKを利用して書き込みを行い既存のブートローダーを破壊してしまった場合、初期状態のArduino開発環境を利用できるようにするには、Adafruitのブートローダーを再度書き込む必要があります。
作業にあたってはJ-LINKが必要となります。

[Arduino Core for Adafruit Bluefruit nRF52 Boards - Burning new Bootloader](https://github.com/adafruit/Adafruit_nRF52_Arduino/#burning-new-bootloader)を参考にして書き込みを行ってください。

※ 正常にArduino IDEから使用できている場合はこの作業を行わないでください。Bootloaderのバージョンを上げる場合は[Bootloaderの更新方法](#bootloaderの更新方法)を参考にしてください。

### Bootloaderの更新方法
Adafruit のページにアップデート方法が記載されています。この通りに行ってください。

https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/updating-the-bootloader
