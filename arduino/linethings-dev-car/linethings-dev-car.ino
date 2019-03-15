#include <bluefruit.h>
#include <Wire.h>
#include <linethings_motor.h>

/**
 * 2WD BLE R/C Car for LINE Things development board with TI DRV8830 motor driver
 */

// Device Name: Maximum 30 bytes
#define DEVICE_NAME "4WD R/C Car DRV8830 - nRF52"

// User service UUID: Change this to your generated service UUID
// #define USER_SERVICE_UUID "8342d390-3478-483c-8bea-6db308de7d04"

// Accelerometer Service UUID
#define RCCAR_SERVICE_UUID "8922e970-329d-44cb-badb-10070ef94b1d"
// [0] Speed: int8, [1] Direction: int8 (right +, left -), [2] Brake: int8 (true, false) 
#define RCCAR_CHARACTERISTIC_UUID "b2a70845-b1d1-4420-b260-fa9551bfe361"

// PSDI Service UUID: Fixed value for Developer Trial
#define PSDI_SERVICE_UUID "E625601E-9E55-4597-A598-76018A0D293D"
#define PSDI_CHARACTERISTIC_UUID "26E2B12B-85F0-4F3F-9FDD-91D114270E6E"

uint8_t userServiceUUID[16];
uint8_t rccarServiceUUID[16];
uint8_t rccarCharacteristicUUID[16];
uint8_t psdiServiceUUID[16];
uint8_t psdiCharacteristicUUID[16];

BLEService userService;
BLEService rccarService;
BLECharacteristic rccarCharacteristic;
BLEService psdiService;
BLECharacteristic psdiCharacteristic;

ThingsMotor motorR(MOTOR_ADDR_CN1); // R
ThingsMotor motorL(MOTOR_ADDR_CN2); // L

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);

  motorR.control(MOTOR_IDLE, 0);
  motorL.control(MOTOR_IDLE, 0);

  setupServices();
  startAdvertising();
  Serial.println("Ready to Connect");
}

void loop() {
  // printMotorFaultStatus();
  delay(500);
}

void motorWriteCallback(BLECharacteristic& chr, uint8_t* data, uint16_t len, uint16_t offset) {
  if (len < 3) {
    return;
  }

  int8_t speed = map((int8_t)data[0], INT8_MIN, INT8_MAX, -100, 100);
  int8_t direction = data[1];
  int8_t brake = data[2];

  int8_t speedL, speedR;
  
  if (direction > 0) {
    // Right turn
    speedL = speed;
    speedR = speed * ((double) (INT8_MAX - direction) / (double) INT8_MAX);
  } else {
    // Left turn
    speedL = speed * ((double) (INT8_MIN - direction) / (double) INT8_MIN);
    speedR = speed;
  }

  Serial.print("S: ");
  Serial.print(speed);
  Serial.print(" (");
  Serial.print(speedL);
  Serial.print(":");
  Serial.print(speedR);
  Serial.print(") ");
  Serial.print(" D: ");
  Serial.print(direction);
  Serial.print(" B: ");
  Serial.println(brake);
  
  if (brake) {
    motorR.control(MOTOR_BRAKE, 0);
    motorL.control(MOTOR_BRAKE, 0);
  } else if (speed > 0) {
    motorR.control(MOTOR_FORWARD, speedR);
    motorL.control(MOTOR_FORWARD, speedL);
  } else if (speed < 0) {
    motorR.control(MOTOR_REVERSE, -speedR);
    motorL.control(MOTOR_REVERSE, -speedL);
  } else {
    motorR.control(MOTOR_IDLE, 0);
    motorL.control(MOTOR_IDLE, 0);
  }
}

void setupServices(void) {
  // Convert String UUID to raw UUID bytes
  strUUID2Bytes(USER_SERVICE_UUID, userServiceUUID);
  strUUID2Bytes(RCCAR_SERVICE_UUID, rccarServiceUUID);
  strUUID2Bytes(RCCAR_CHARACTERISTIC_UUID, rccarCharacteristicUUID);
  strUUID2Bytes(PSDI_SERVICE_UUID, psdiServiceUUID);
  strUUID2Bytes(PSDI_CHARACTERISTIC_UUID, psdiCharacteristicUUID);

  // Setup User Service
  userService = BLEService(userServiceUUID);
  userService.begin();

  rccarService = BLEService(rccarServiceUUID);
  rccarService.begin();

  rccarCharacteristic = BLECharacteristic(rccarCharacteristicUUID);
  rccarCharacteristic.setProperties(CHR_PROPS_WRITE);
  rccarCharacteristic.setWriteCallback(motorWriteCallback);
  rccarCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  rccarCharacteristic.setFixedLen(3);
  rccarCharacteristic.begin();

  // Setup PSDI Service
  psdiService = BLEService(psdiServiceUUID);
  psdiService.begin();

  psdiCharacteristic = BLECharacteristic(psdiCharacteristicUUID);
  psdiCharacteristic.setProperties(CHR_PROPS_READ);
  psdiCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  psdiCharacteristic.setFixedLen(sizeof(uint32_t) * 2);
  psdiCharacteristic.begin();

  // Set PSDI (Product Specific Device ID) value
  uint32_t deviceAddr[] = { NRF_FICR->DEVICEADDR[0], NRF_FICR->DEVICEADDR[1] };
  psdiCharacteristic.write(deviceAddr, sizeof(deviceAddr));
}

void startAdvertising(void) {
  // Start Advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(userService);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);
}

// UUID Converter
void strUUID2Bytes(String strUUID, uint8_t binUUID[]) {
  String hexString = String(strUUID);
  hexString.replace("-", "");

  for (int i = 16; i != 0 ; i--) {
    binUUID[i - 1] = hex2c(hexString[(16 - i) * 2], hexString[((16 - i) * 2) + 1]);
  }
}

char hex2c(char c1, char c2) {
  return (nibble2c(c1) << 4) + nibble2c(c2);
}

char nibble2c(char c) {
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  if ((c >= 'A') && (c <= 'F'))
    return c + 10 - 'A';
  if ((c >= 'a') && (c <= 'f'))
    return c + 10 - 'a';
  return 0;
}
