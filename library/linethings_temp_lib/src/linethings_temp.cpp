#include "linethings_temp.h"

ThingsTemp::ThingsTemp(uint8_t addr) : deviceAddr(addr) {
}

void ThingsTemp::init() {
  Wire.begin();
  Wire.beginTransmission(deviceAddr);
  Wire.write(0x01);
  Wire.write(0x60);       // 12bitで温度を取得
  Wire.write((byte)0x00);
  Wire.endTransmission();
  delay(100);
}

float ThingsTemp::read() {
  byte data[2];
  int m_data;
  Wire.beginTransmission(deviceAddr);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  delay(300);

  Wire.requestFrom(deviceAddr, 2);
  if (Wire.available() == 2) {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }

  m_data = ((data[0] << 8) + (data[1] & 0xf0)) >> 4;
  if (m_data > 2047) {
    m_data = m_data - 4096;
  }
  return m_data * 0.0625;
}
