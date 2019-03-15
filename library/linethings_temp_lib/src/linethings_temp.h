#ifndef THINGS_TEMP_H
#define THINGS_TEMP_H

#include <Arduino.h>
#include <Wire.h>

#define AT30TS74_ADDR 0x48

class ThingsTemp {
  public:
    ThingsTemp(byte addr = AT30TS74_ADDR);
    void init();
    float read();
  private:
    byte deviceAddr;
};

#endif // THINGS_TEMP_H
