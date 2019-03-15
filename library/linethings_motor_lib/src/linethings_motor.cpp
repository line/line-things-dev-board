#include "linethings_motor.h"

ThingsMotor::ThingsMotor(uint8_t addr) : deviceAddr(addr) {
}

void ThingsMotor::init() {
  Wire.begin();
}

byte ThingsMotor::readStatus() {
  return read(1);
}

void ThingsMotor::clearFault() {
  write(1, 0x80);
}

void ThingsMotor::control(byte cmd, byte speed) {
  if (speed == 0) {
    write(0, 0 | MOTOR_IDLE);
    return;
  }
  byte tx_speed = map(speed, 1, 100, 6, 64);
  write(0, tx_speed << 2 | cmd);
}

void ThingsMotor::write(byte address, byte data) {
  Wire.beginTransmission(deviceAddr);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission(true);
}

byte ThingsMotor::read(byte address) {
  Wire.beginTransmission(deviceAddr);
  Wire.write(address);
  Wire.endTransmission();

  return Wire.read();
}
