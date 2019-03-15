#ifndef THINGS_MOTOR_H
#define THINGS_MOTOR_H

#include <Arduino.h>
#include <Wire.h>

#define MOTOR_ADDR_CN1 0x60
#define MOTOR_ADDR_CN2 0x62

#define MOTOR_IDLE 0
#define MOTOR_REVERSE 1
#define MOTOR_FORWARD 2
#define MOTOR_BRAKE 3

class ThingsMotor {
  public:
    ThingsMotor(byte addr = MOTOR_ADDR_CN1);
    void init();
    void control(byte cmd, byte speed);      // Speed 0~100, when 0 same as Idle state
    byte readStatus();
    void clearFault();
  private:
    byte deviceAddr;
    void write(byte address, byte data);
    byte read(byte address);
};

#endif // THINGS_MOTOR_H
