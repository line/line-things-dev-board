/**
 * サンプルコード - Temperature sensor
 * 温度を取得するサンプルコード
 * library/things_temp_lib.zipをライブラリを管理からインストールしてください
 */

#include <linethings_temp.h>

ThingsTemp temp = ThingsTemp();

void setup() {
  temp.init();
  Serial.begin(9600);
}

void loop() {
  Serial.println(temp.read());
}
