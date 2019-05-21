# デフォルトファームウェアについて

## GATT サービスの仕様
デフォルトファームウェアに実装されているGATTサービス・キャラクタリスティックの仕様について説明します。
ここからの説明は[JavaScriptライブラリ](js-control.md)を使用する際には不要ですが、拡張したり、LIFF以外からデバイスを制御する場合に必要な情報です。

### Characteristic の説明
PSDIを除く全てのCharacteristicは、以下のサービスに実装されています。

- `DEVBOARD_SERVICE_UUID`
  - `f2b742dc-35e3-4e55-9def-0ce4a209c552`

このサービスに実装されている Characteristic と UUID は以下の通りです。

|説明 | 名称 | UUID | 属性 | サイズ(固定長) |
----|----|----|----|----
| 1 | WRITE_BOARD_STATE_CHARACTERISTIC_UUID | 4f2596d7-b3d6-4102-85a2-947b80ab4c6f | Write | 20Byte |
| 2 | VERSION_CHARACTERISTIC_UUID | be25a3fe-92cd-41af-aeee-0a9097570815 | Read | 1Byte |
| 3 | NOTIFY_SW_CHARACTERISTIC_UUID | a11bd5c0-e7da-4015-869b-d5c0087d3cc4 | Notify | 2Byte |
| 4 | NOTIFY_TEMP_CHARACTERISTIC_UUID | fe9b11a8-5f98-40d6-ae82-bea94816277f | Notify | 2Byte |
| 5 | COMMAND_WRITE_CHARACTERISTIC_UUID | 5136e866-d081-47d3-aabc-a2c9518bacd4 | Write | 18Byte |
| 6 | COMMAND_RESPONSE_CHARACTERISTIC_UUID | 1737f2f4-c3d3-453b-a1a6-9efe69cc944f  | Read + Notify | 4Byte |

1. JavaScriptライブラリ内では、Service UUIDの書き換えのみで使用しています。
2. Dev boardのファームウェアのバージョンを取得します。デバイス側で定義されていない場合はVersion 1とします。
3. Switchのnotifyが有効化されているとき、スイッチイベントが発生するときにnotifyします。
4. 温度センサのnotifyが有効化されているとき、一定間隔で温度をnotifyします。
5. Dev boardのデバイスの設定や、notifyの設定など、コマンドを書き込むために使用します。
6. Dev boardから、コマンドに対応したデータがある場合、それを取得するために利用します。Notify にも対応しています。

### WRITE_BOARD_STATE_CHARACTERISTIC_UUID (Service UUIDの書き換え)
Service UUIDの書き換えを行うためのキャラクタリスティックです。
`Write device`はボード上のデバイスの値を書き換えることができますが、これは `/liff-app/linethings-dev-default` が使用しているものです。

フォーマット

| |Command (1Byte) | reserved (2Byte) | hash (1Byte) | payload (16Byte) | 説明 |
----|----|----|----|----|----
| Write device | 0 | {0, 0} | 0 | see code | ボード上のデバイスとGPIOを一括設定する際に使用します。Version 1から使用できます|
| Write Service UUID | 1 | {0, 0} | hash of uuid | uuid | hashはUUIDをリダクション加算した値を使用します。UUIDは"-"は省略します |

### VERSION_CHARACTERISTIC_UUID (ファームウェアのバージョン取得)
Firmwareのバージョンを取得します。Version 1ではこのキャラクタリスティックが実装されていませんでした。
そのため、LIFF側からこのキャラクタリスティックのReadがerrorとなった場合は
Version 1であるとして、Version 2以降で追加となったService UUIDの書き換え機能以外は非対応としなくてはいけません。

### NOTIFY_SW_CHARACTERISTIC_UUID (SW Notify)
スイッチが押されたタイミングでnotifyをします。
`DEFAULT_CHARACTERISTIC_IO_WRITE_UUID`にてSWのnotifyの設定が可能です。
初期状態では、source = SW1 & SW2, mode = `CHANGE`, interval = 50ms に設定されています。

フォーマット

|value[1]|value[0]|
----|----
|address(1:SW1, 2:SW2)|SW value|

### NOTIFY_TEMP_CHARACTERISTIC_UUID (Temperature Notify)
`DEFAULT_CHARACTERISTIC_IO_WRITE_UUID`にて温度のnotitfyが有効化された場合、設定されたタイミングでnotifyをします。
notifyされる16Byteの温度データは100倍されたものが送信されます。

初期状態では、source = 1, interval = 10000ms に設定されています。

### COMMAND_WRITE_CHARACTERISTIC_UUID (デバイス操作)
ボード上のデバイスの設定や、Notifyの設定などを行います。

フォーマット

| |Command (1Byte) | Payload (17Byte) | 説明 |
----|----|----|----
| Control display | 0 | {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, address_x, address_y} | Write textで使用するindexアドレスを指定します | |
| Write text | 1 | {length of text, (text max 16Byte)} | 任意のテキストを書き込み | Control displayで設定したアドレスは自動的にインクリメントされません|
| Clear display| 2| don't care | ディスプレイをクリア| |
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

### COMMAND_RESPONSE_CHARACTERISTIC_UUID (デバイスからread)
`COMMAND_WRITE_CHARACTERISTIC_UUID`にて設定されたデバイスの値を取得します。設定されたデバイスソースによって、送信されるデータが異なります。

|Source|Value format|
----|----
|Switch|{0, 0, SW2, SW1}|
|Accel| {0, X, Y, Z} |
|Temperature|温度が100倍された値 |
|GPIO Digital| {0, 0, 0, value} |
|GPIO Analog| {0, 0, 0, value}|
|I2C| {0, 0, (0:invalid / 1:valid), value} |
