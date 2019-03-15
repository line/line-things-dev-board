/**
 * サンプルコード - Motor driver DRV8830
 * 2つのモーターを制御するサンプルです
 * library/things_motor_lib.zipをライブラリを管理からインストールしてください
 */

#include <linethings_motor.h>

ThingsMotor motor_cn1 = ThingsMotor(MOTOR_ADDR_CN1);
ThingsMotor motor_cn2 = ThingsMotor(MOTOR_ADDR_CN2);

void setup() {
  motor_cn1.init();
  motor_cn2.init();
}

int i = 0;

void loop() {
  motor_cn1.control(MOTOR_BACK, i);
  motor_cn2.control(MOTOR_FORWARD, i);
  delay(100);
  i++;
  if (i >= 100) {
    i = 0;
  }
}
