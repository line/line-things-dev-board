/**
 * Default firmware for LINE Things development board
 * This firmware suppoted 2 type of operation.
 * - Demo mode (NOTIFY_BOARD_STATE_CHARACTERISTIC, WRITE_BOARD_STATE_CHARACTERISTIC)
 *    In this mode, you can get or set whole board state of the LINE Things dev board at once.
 *    Used in combination with LIFF in /liff-app/linethings-dev-default of repository.
 *
 * - Command control mode / Notify switch, temperature state
 *    In this mode, all sensors and individual GPIOs can be controlled from command characteristic.
 *    Easy to hack dev board from LIFF and automatic communication scenario without firmware changes.
 *    Used in combination with Javascript library for LIFF in /liff-app/js-control of repository.
 *    See here for details.
 *    https://line.github.io/line-things-dev-board/liff-app/js-control/
 */

#include <bluefruit.h>
#include <Bluefruit_FileIO.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_MMA8452Q.h>
#include <linethings_temp.h>

// Debug On / Off
//#define USER_DEBUG
// BLE Default Advertising UUID
#define DEFAULT_ADVERTISE_UUID "f2b742dc-35e3-4e55-9def-0ce4a209c552"
// BLE Service UUID
#define DEVBOARD_SERVICE_UUID "f2b742dc-35e3-4e55-9def-0ce4a209c552"
#define NOTIFY_BOARD_STATE_CHARACTERISTIC_UUID "e90b4b4e-f18a-44f0-8691-b041c7fe57f2"
#define WRITE_BOARD_STATE_CHARACTERISTIC_UUID "4f2596d7-b3d6-4102-85a2-947b80ab4c6f"
#define VERSION_CHARACTERISTIC_UUID "be25a3fe-92cd-41af-aeee-0a9097570815"
#define COMMAND_WRITE_CHARACTERISTIC_UUID "5136e866-d081-47d3-aabc-a2c9518bacd4"
#define COMMAND_RESPONSE_CHARACTERISTIC_UUID "1737f2f4-c3d3-453b-a1a6-9efe69cc944f"
#define NOTIFY_SW_CHARACTERISTIC_UUID "a11bd5c0-e7da-4015-869b-d5c0087d3cc4"
#define NOTIFY_TEMP_CHARACTERISTIC_UUID "fe9b11a8-5f98-40d6-ae82-bea94816277f"
#define PSDI_SERVICE_UUID "e625601e-9e55-4597-a598-76018a0d293d"
#define PSDI_CHARACTERISTIC_UUID "26e2b12b-85f0-4f3f-9fdd-91d114270e6e"
// Device name and version
#define BLE_DEV_NAME "LINE Things dev board"
#define FIRMWARE_VERSION 2
// Device and pin
#define SW1 29
#define SW2 28
#define LED_DS2 7
#define LED_DS3 11
#define LED_DS4 19
#define LED_DS5 17
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define GPIO5 5
#define GPIO12 12
#define GPIO13 13
#define GPIO14 14
#define GPIO15 15
#define GPIO16 16
#define BUZZER_PIN 27

/*********************************************************************************
* Internal config file
*********************************************************************************/
#define UUID_FILENAME "/uuidconfig.txt"
File file(InternalFS);

/*********************************************************************************
* I2C Peripherals
*********************************************************************************/
// I2S Display
Adafruit_SSD1306 display(128, 64, &Wire, -1);
// Acceleration sensor. (MMA8452)
MMA8452Q accel(0x1C);
// Temperature sensor (AT30TS74)
ThingsTemp temp = ThingsTemp();

/*********************************************************************************
* Buzzer
*********************************************************************************/
SoftwareTimer buzzer;

// Callback for buzzer control @1khz
void buzzerEvent(TimerHandle_t xTimerID) {
  digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
}

void buzzerStart() {
  pinMode(BUZZER_PIN, OUTPUT);
  buzzer.begin(1, buzzerEvent);
  buzzer.start();
}

void buzzerStop() {
  buzzer.stop();
  digitalWrite(BUZZER_PIN, 0);
}

/*********************************************************************************
* Debug print
*********************************************************************************/
void debugPrint(String text) {
#ifdef USER_DEBUG
  text = "[DBG]" + text;
  Serial.println(text);
#endif
}

/*********************************************************************************
* BLE settings
*********************************************************************************/
// Advertising service UUID
uint8_t blesv_user_uuid[16];
BLEService blesv_user = BLEService(blesv_user_uuid);

// LINE Things PSDI service
uint8_t blesv_line_uuid[16];
uint8_t blesv_line_product_uuid[16];
BLEService blesv_line = BLEService(blesv_line_uuid);
BLECharacteristic blesv_line_product = BLECharacteristic(blesv_line_product_uuid);

// LINE Things development board service
uint8_t blesv_devboard_uuid[16];
uint8_t blech_notify_board_state_uuid[16];
uint8_t blech_write_board_state_uuid[16];
uint8_t blech_version_uuid[16];
uint8_t blech_command_write_uuid[16];
uint8_t blech_command_response_uuid[16];
uint8_t blech_notify_sw_uuid[16];
uint8_t blech_notify_temp_uuid[16];
BLEService blesv_devboard = BLEService(blesv_devboard_uuid);
BLECharacteristic blech_notify_board_state = BLECharacteristic(blech_notify_board_state_uuid);
BLECharacteristic blech_write_board_state = BLECharacteristic(blech_write_board_state_uuid);
BLECharacteristic blech_version = BLECharacteristic(blech_version_uuid);
BLECharacteristic blech_command_write = BLECharacteristic(blech_command_write_uuid);
BLECharacteristic blech_command_response = BLECharacteristic(blech_command_response_uuid);
BLECharacteristic blech_notify_sw = BLECharacteristic(blech_notify_sw_uuid);
BLECharacteristic blech_notify_temp = BLECharacteristic(blech_notify_temp_uuid);

// UUID Converter
void strUUID2Bytes(String strUUID, uint8_t binUUID[]) {
  String hexString = String(strUUID);
  hexString.replace("-", "");

  for (int i = 16; i != 0; i--) {
    binUUID[i - 1] = hex2c(hexString[(16 - i) * 2], hexString[((16 - i) * 2) + 1]);
  }
}

char hex2c(char c1, char c2) { return (nibble2c(c1) << 4) + nibble2c(c2); }

char nibble2c(char c) {
  if ((c >= '0') && (c <= '9')) return c - '0';
  if ((c >= 'A') && (c <= 'F')) return c + 10 - 'A';
  if ((c >= 'a') && (c <= 'f')) return c + 10 - 'a';
  return 0;
}

void bleConfigure(int power) {
  // UUID setup
  strUUID2Bytes(PSDI_SERVICE_UUID, blesv_line_uuid);
  strUUID2Bytes(PSDI_CHARACTERISTIC_UUID, blesv_line_product_uuid);
  strUUID2Bytes(DEVBOARD_SERVICE_UUID, blesv_devboard_uuid);
  strUUID2Bytes(NOTIFY_BOARD_STATE_CHARACTERISTIC_UUID, blech_notify_board_state_uuid);
  strUUID2Bytes(WRITE_BOARD_STATE_CHARACTERISTIC_UUID, blech_write_board_state_uuid);
  strUUID2Bytes(VERSION_CHARACTERISTIC_UUID, blech_version_uuid);
  strUUID2Bytes(COMMAND_RESPONSE_CHARACTERISTIC_UUID, blech_command_response_uuid);
  strUUID2Bytes(COMMAND_WRITE_CHARACTERISTIC_UUID, blech_command_write_uuid);
  strUUID2Bytes(NOTIFY_SW_CHARACTERISTIC_UUID, blech_notify_sw_uuid);
  strUUID2Bytes(NOTIFY_TEMP_CHARACTERISTIC_UUID, blech_notify_temp_uuid);
  // BLE start
  Bluefruit.begin();
  // Set max Tx power
  // Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(power);
  // BLE devicename
  Bluefruit.setName(BLE_DEV_NAME);
  Bluefruit.Periph.setConnInterval(12, 1600); // connection interval min=20ms, max=2s
  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(bleConnectEvent);
  Bluefruit.Periph.setDisconnectCallback(bleDisconnectEvent);
}

void bleStartAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.setFastTimeout(0);
  Bluefruit.Advertising.setInterval(32, 32); // interval : 20ms
  Bluefruit.Advertising.restartOnDisconnect(true);
  // Add service uuid for find from LINE app
  Bluefruit.Advertising.addUuid(BLEUuid(blesv_user_uuid));
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.start();
}

void bleSetupServicePsdi(void) {
  blesv_line.begin();
  blesv_line_product.setProperties(CHR_PROPS_READ);
  blesv_line_product.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  blesv_line_product.setFixedLen(sizeof(uint32_t) * 2);
  blesv_line_product.begin();
  uint32_t deviceAddr[] = {NRF_FICR->DEVICEADDR[0], NRF_FICR->DEVICEADDR[1]};
  blesv_line_product.write(deviceAddr, sizeof(deviceAddr));
}

void bleSetupServiceDevice() {
  blesv_devboard.begin();
  blech_notify_board_state.setProperties(CHR_PROPS_NOTIFY);
  blech_notify_board_state.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  blech_notify_board_state.setFixedLen(16);
  blech_notify_board_state.begin();

  blech_write_board_state.setProperties(CHR_PROPS_WRITE);
  blech_write_board_state.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  blech_write_board_state.setWriteCallback(bleWriteEvent);
  blech_write_board_state.setFixedLen(20);
  blech_write_board_state.begin();

  blech_version.setProperties(CHR_PROPS_READ);
  blech_version.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  blech_version.setFixedLen(1);
  blech_version.begin();
  blech_version.write8(FIRMWARE_VERSION);

  blech_command_write.setProperties(CHR_PROPS_WRITE);
  blech_command_write.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  blech_command_write.setWriteCallback(bleIoWriteEvent);
  blech_command_write.setFixedLen(20);
  blech_command_write.begin();

  blech_command_response.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  blech_command_response.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  blech_command_response.setFixedLen(4);
  blech_command_response.begin();

  blech_notify_sw.setProperties(CHR_PROPS_NOTIFY);
  blech_notify_sw.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  blech_notify_sw.setFixedLen(2);
  blech_notify_sw.begin();

  blech_notify_temp.setProperties(CHR_PROPS_NOTIFY);
  blech_notify_temp.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  blech_notify_temp.setFixedLen(2);
  blech_notify_temp.begin();
}

void bleSetupServiceUser() {
  blesv_user.begin();
}

// Event for connect BLE central
void bleConnectEvent(uint16_t conn_handle) {
  char central_name[32] = {0};

  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  connection->getPeerName(central_name, sizeof(central_name));

  String msg = "Connected from " + String(central_name);
  debugPrint(msg);
}

// Event for disconnect BLE central
void bleDisconnectEvent(uint16_t conn_handle, uint8_t reason) {
  (void)reason;
  (void)conn_handle;
  debugPrint("BLE central disconnect");
}

volatile int g_flag_user_write = 0;
volatile int g_data_user_write_led0 = 0;
volatile int g_data_user_write_led1 = 0;
volatile int g_data_user_write_led2 = 0;
volatile int g_data_user_write_led3 = 0;
volatile int g_data_user_write_buzzer = 0;
volatile int g_data_user_write_io2 = 0;
volatile int g_data_user_write_io3 = 0;
volatile int g_data_user_write_io4 = 0;
volatile int g_data_user_write_io5 = 0;
volatile int g_data_user_write_io12 = 0;
volatile int g_data_user_write_io13 = 0;
volatile int g_data_user_write_io14 = 0;
volatile int g_data_user_write_io15 = 0;
volatile int g_data_user_write_io16 = 0;

volatile int g_flag_update_advertiseuuid = 0;
volatile int g_flag_error_advertiseuuid = 0;

/**
 * Format
 *
 * <CMD(1Byte), don't care(2Byte), hash of payload/don't care(1Byte),
 * Payload(16Byte)> CMD0 : Write to peripheral device. LED, buzzer and GPIO
 *    CMD0(0:1Byte), don't care(0:17Byte), Peripheral(x:2Byte)
 * CMD1 : Write new service UUID : When update UUID, Should be self restart MPU:
 *    CMD1(1:1Byte), don't care(0,2Byte), hash of payload(x:1Byte),
 *    UUID(x:16Byte) UUID shoud be send binary.
 */
void bleWriteEvent(uint16_t conn_handle, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  byte cmd = data[0];
  byte index = data[1];
  byte length = data[2];
  byte hash = data[3];

  Serial.println("UUID Change ");

  if (cmd == 0) {
    // Write peripheral device and GPIO
    g_data_user_write_led0 = (data[19] >> 7) & 1;
    g_data_user_write_led1 = (data[19] >> 6) & 1;
    g_data_user_write_led2 = (data[19] >> 5) & 1;
    g_data_user_write_led3 = (data[19] >> 4) & 1;
    g_data_user_write_buzzer = (data[19] >> 3) & 1;
    g_data_user_write_io2 = (data[19] >> 2) & 1;
    g_data_user_write_io3 = (data[19] >> 1) & 1;
    g_data_user_write_io4 = data[19] & 1;
    g_data_user_write_io5 = (data[18] >> 7) & 1;
    g_data_user_write_io12 = (data[18] >> 6) & 1;
    g_data_user_write_io13 = (data[18] >> 5) & 1;
    g_data_user_write_io14 = (data[18] >> 4) & 1;
    g_data_user_write_io15 = (data[18] >> 3) & 1;
    g_data_user_write_io16 = (data[18] >> 2) & 1;
    g_flag_user_write = 1;
  } else if (cmd == 1) {
    // Write New Advertising UUID
    byte generatedHash = 0;
    for (int i = 0; i < 16; i++) {
      blesv_user_uuid[15 - i] = data[4 + i];
      generatedHash += data[4 + i];
    }

    g_flag_update_advertiseuuid = 1;

    if (generatedHash != hash) {
      g_flag_error_advertiseuuid = 1;
    }
  }
}

typedef struct ble_io_action {
  byte changed = 0;
  int port = 0;
  int value = 0;
} bleIoAction;

typedef struct ble_display_action {
  byte changed = 0;
  int addr_x = 0;
  int addr_y = 0;
} bleDispAction;

typedef struct ble_display_text_action {
  byte changed = 0;
  char text[16];
  byte length = 0;
} bleDispTextAction;

typedef struct ble_i2c_action {
  byte changed = 0;
  byte data = 0;
  byte length = 0;
} bleI2cAction;

typedef struct ble_read_action {
  byte changed = 0;
  byte cmd = 0;
} bleReadAction;

typedef struct ble_notify_action {
  byte changed;
  byte source;
  byte level;
  byte mode;
  unsigned int interval;
} bleNotifyAction;

volatile bleIoAction g_led;
volatile bleIoAction g_led_byte;
volatile bleIoAction g_buzzer;

volatile bleIoAction g_gpio_dir;
volatile bleIoAction g_gpio_w_value;
volatile bleIoAction g_gpio_aw_value;

volatile bleDispAction g_display;
volatile int g_display_clear = 0;
volatile bleDispTextAction g_display_text;
volatile int g_display_fontsize = 0;

volatile bleI2cAction g_i2c_start_transmission;
volatile bleI2cAction g_i2c_w_value;
volatile int g_i2c_stop_transmission;
volatile bleI2cAction g_i2c_request_from;
volatile int g_i2c_read_req;

volatile int g_gpio_r_port;
volatile int g_gpio_ar_port;

volatile bleReadAction g_read_action;

volatile bleNotifyAction g_notify_sw = {
  .changed = 1, .source = 3, .level = 0, .mode = 1, .interval = 50
};
volatile bleNotifyAction g_notify_temp = {
  .changed = 1, .source = 1, .level = 0, .mode = 1, .interval = 10000
};

volatile int g_display_user_mode = 0;
volatile int g_i2c_user_mode = 0;

/**
 * Write to device format
 * See here for details.
 *    https://line.github.io/line-things-dev-board/liff-app/js-control/
 *
 * <CMD(1Byte), Payload(17Byte)>
 *  CMD0 : Control display
 *    Payload : don't care(15Byte), address_x(1Byte), address_y(1Byte)
 *  CMD1 : Write Text
 *    Payload : text length(1Byte), String(16Byte)
 *  CMD2 : Clear display
 *    Payload : don't care(17Byte)
 *  CMD3 : Write LED
 *    Payload : don't care(15Byte), port(1Byte), value(1Byte)
 *  CMD4 : Write Buzzer
 *    Payload : don't care(16Byte), value(1Byte)
 *  CMD5 : Set GPIO digital direction
 *    Payload : don't care(15Byte), port(1Byte), direction(1Byte)
 *  CMD6 : Digital Write GPIO
 *    Payload : don't care(15Byte), port(1Byte), value(1Byte)
 *  CMD7 : Analog Write GPIO
 *    Payload : don't care(15Byte), port(1Byte), value(1Byte)
 *  CMD8 : I2C Start transmission
 *    Payload : don't care(16Byte), address(1Byte)
 *  CMD9 : I2C Write data
 *    Payload : don't care(16Byte), address(1Byte)
 *  CMD10 : I2C Stop transmission
 *    Payload : don't care(17Byte)
 *  CMD11 : I2C Request from
 *    Payload : don't care(15Byte), length[1Byte], saddress(1Byte)
 *  CMD12 : I2C Read request
 *    Payload : don't care(17Byte)
 *  CMD13 : Set port for read digital value
 *    Payload : don't care(16Byte), port(1Byte)
 *  CMD14 : Set port for read analog value
 *    Payload : don't care(16Byte), port(1Byte)
 *  CMD15 : Display write font size
 *    Payload : don't care(16Byte), fontsize(1Byte)
 *  CMD16 : Write LED (Byte)
 *    Payload : don't care(16Byte), value(1Byte)
 *  CMD17 : SW Notify Config
 *    Payload : source(1Byte), triger_value(1Byte), mode(1Byte), interval(2Byte)
 *      *source:
 *        0:disable, 1:SW1, 2:SW2, 3:SW1&SW2
 *      *mode
 *        0:LOW, 1:CHANGE, 2:RISING, 3:FALLING
 *      *interval
 *        0~65535:0~65535mS
 *  CMD18 : Temperature Notify Config
 *    Payload : source(1Byte), interval(2Byte)
 *      *source:
 *        0:disable 1:temperature
 *      *interval
 *        0~65535:0~65535mS
 *  CMD32 : Set BLE Read data
 *    Payload : don't care(16Byte), Read CMD(1Byte)
 *
 * Read to device format(Read CMD)
 * <Payload(4Byte)>
 *  CMD0 : Switch status
 *    Payload : don't care(3Byte), switch value(1Byte)
 *  CMD1 : Accel value
 *    Payload : don't care(1Byte), X(1Byte), Y(1Byte), Z(1Byte)
 *  CMD2 : Temperature value
 *    Payload : don't care(2Byte), value(2Byte)
 *  CMD3 : Read Digital GPIO value (using by BLE write pointer CMD13)
 *    Payload : don't care(3Byte), value(1Byte)
 *  CMD4 : Read Analog GPIO value (using by BLE write pointer CMD14)
 *    Payload : don't care(3Byte), value(1Byte)
 *  CMD5 : I2C Read
 *    Payload : don't care(2Byte), flag(1Byte), value(1Byte)
 */
void bleIoWriteEvent(uint16_t conn_handle, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  byte cmd = data[0];
  switch (cmd) {
    case 0:
      g_display_user_mode = 1;
      g_display.changed = 1;
      g_display.addr_x = data[16];
      g_display.addr_y = data[17];
      break;
    case 1:
      g_display_user_mode = 1;
      g_display_text.changed = 1;
      g_display_text.length = data[1];
      for (int i = 0; i < 16; i++) {
        g_display_text.text[i] = data[2 + i];
      }
      break;
    case 2:
      g_display_user_mode = 1;
      g_display_clear = 1;
      break;
    case 3:
      g_led.changed = 1;
      g_led.port = data[16];
      g_led.value = data[17];
      break;
    case 4:
      g_buzzer.changed = 1;
      g_buzzer.port = data[16];
      g_buzzer.value = data[17];
      break;
    case 5:
      g_gpio_dir.changed = 1;
      g_gpio_dir.port = data[16];
      g_gpio_dir.value = data[17];
      break;
    case 6:
      g_gpio_w_value.changed = 1;
      g_gpio_w_value.port = data[16];
      g_gpio_w_value.value = data[17];
      break;
    case 7:
      g_gpio_aw_value.changed = 1;
      g_gpio_aw_value.port = data[16];
      g_gpio_aw_value.value = data[17];
      break;
    case 8:
      g_i2c_start_transmission.changed = 1;
      g_i2c_start_transmission.data = data[17];
      break;
    case 9:
      g_i2c_w_value.changed = 1;
      g_i2c_w_value.data = data[17];
      break;
    case 10:
      g_i2c_stop_transmission = 1;
      break;
    case 11:
      g_i2c_request_from.changed = 1;
      g_i2c_request_from.data = data[17];
      g_i2c_request_from.length = data[16];
      break;
    case 12:
      g_i2c_read_req = 1;
      break;
    case 13:
      g_gpio_r_port = data[17];
      break;
    case 14:
      g_gpio_ar_port = data[17];
      break;
    case 15:
      g_display_user_mode = 1;
      g_display_fontsize = data[17];
      break;
    case 16:
      g_led_byte.changed = 1;
      g_led_byte.value = data[17];
      break;
    case 17:
      g_notify_sw.changed = 1;
      g_notify_sw.source = data[14];
      g_notify_sw.level = 0;
      g_notify_sw.mode = data[15];
      g_notify_sw.interval = data[16] * 256 + data[17];
      break;
    case 18:
      g_notify_temp.changed = 1;
      g_notify_temp.source = data[15];
      g_notify_temp.level = 0;
      g_notify_temp.mode = 0;
      g_notify_temp.interval = data[16] * 256 + data[17];
      break;
    case 32:
      g_read_action.changed = 1;
      g_read_action.cmd = data[17];
      break;
    default:
      break;
  }
}

/*********************************************************************************
* Timer
*********************************************************************************/
volatile int g_flag_display = 0;
SoftwareTimer timerDisplay;
void displayUpdateEvent(TimerHandle_t xTimerID) { g_flag_display = 1; }

volatile int g_flag_notify = 0;
SoftwareTimer timerNotify;
void bleNotifyEvent(TimerHandle_t xTimerID) { g_flag_notify = 1; }

volatile int g_flag_io_temp_read = 0;
SoftwareTimer io_notify_temp_interval;
void bleIoNotifyTempEvent(TimerHandle_t xTimerID) { g_flag_io_temp_read = 1; }

/*********************************************************************************
* SW Event
*********************************************************************************/
volatile byte g_flag_sw1 = 0;
volatile byte g_data_sw1 = 0;
volatile unsigned long g_last_notified_sw1 = 0;
void sw1ChangedEvent() {
  byte value = !digitalRead(SW1);
  if (g_notify_sw.source & 1) {
    g_data_sw1 = value;
    g_flag_sw1++;
  }
}

volatile byte g_flag_sw2 = 0;
volatile byte g_data_sw2 = 0;
volatile unsigned long g_last_notified_sw2 = 0;
void sw2ChangedEvent() {
  byte value = !digitalRead(SW2);
  if (g_notify_sw.source & 2) {
    g_data_sw2 = value;
    g_flag_sw2++;
  }
}

/*********************************************************************************
* UUID Configure data
*********************************************************************************/
void configFileWrite(uint8_t binUuid[]) {
  int i = 0;
  if (file.open(UUID_FILENAME, FILE_WRITE)) {
    file.seek(0);

    for (i = 0; i < 16; i++) {
      file.write(binUuid[i]);
    }
    file.close();
  } else {
    debugPrint("[UUID]Write UUID : Failed!");
  }
}

void configFileRead() {
  file.open(UUID_FILENAME, FILE_READ);
  file.read(blesv_user_uuid, sizeof(blesv_user_uuid));
  file.close();
}

int configFileExist() {
  file.open(UUID_FILENAME, FILE_READ);
  if (!file) {
    file.close();
    return -1;
  }
  file.close();
  return 0;
}

int compareUuid(uint8_t uuid1[], uint8_t uuid2[]) {
  for (int i = 0; i < 16; i++) {
    if (uuid1[i] != uuid2[i]) {
      return -1;
    }
  }
  return 0;
}

/*********************************************************************************
* Control - On board devices
*********************************************************************************/
void displayClear() {
  debugPrint("[BLE]DISP : clear display");
  display.clearDisplay();
  display.display();
}

void displaySetConfigure(int addr_x, int addr_y) {
  debugPrint("[BLE]DISP : write configure");
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(addr_x, addr_y);
}

void displayWrite(int fontsize, int length, char ch[16]) {
  debugPrint("[BLE]DISP : write text");
  display.setTextSize(fontsize);
  for (int i = 0; i < length; i++) {
    display.print(ch[i]);
  }
  display.display();
}

unsigned int swGetValue(unsigned int sw) {
  debugPrint("[BLE]SW : read switch value");
  unsigned int pin;
  if (sw == 1) {
    pin = SW1;
  } else {
    pin = SW2;
  }
  return digitalRead(pin);
}

void ledWrite(unsigned int num, unsigned data) {
  debugPrint("[BLE]LED : write led value");
  unsigned int pin;
  switch (num) {
    case 2:
      pin = LED_DS2;
      break;
    case 3:
      pin = LED_DS3;
      break;
    case 4:
      pin = LED_DS4;
      break;
    case 5:
      pin = LED_DS5;
      break;
    default:
      pin = LED_DS2;
      break;
  }
  digitalWrite(pin, data);
}

void buzzerWrite(unsigned int data) {
  debugPrint("[BLE]BUZZER : write buzzer");
  if(data) {
    buzzerStart();
  }else{
    buzzerStop();
  }
}

float tempRead() {
  debugPrint("[BLE]Temperature : read value");
  return temp.read();
}

void accelRead(float data[3]) {
  debugPrint("[BLE]Accel : read value");
  accel.read();
  data[0] = accel.x;
  data[1] = accel.y;
  data[2] = accel.z;
}

void i2cBeginTransmission(byte address) {
  String msg = "[BLE]I2C : begin transaction, Address=" + String(address);
  Wire.beginTransmission(address);
  debugPrint(msg);
};

void i2cWrite(byte data) {
  String msg = "[BLE]I2C : write data, data=" + String(data);
  Wire.write(data);
  debugPrint(msg);
};

void i2cEndTransmission() {
  debugPrint("[BLE]I2C : end transaction");
  Wire.endTransmission();
}

void i2cRequestFrom(byte address, byte length) {
  String msg = "[BLE]I2C : request from, address=" + String(address) + ", length=" + String(length);
  Wire.requestFrom(address, length);
  debugPrint(msg);
}

byte i2cRead() {
  byte value;
  value = Wire.read();
  String msg = "[BLE]I2C : read -> " + String(value);
  debugPrint(msg);
  return value;
}

/*********************************************************************************
* Control - IO
*********************************************************************************/
void ioDigitalDir(unsigned int pin, unsigned int dir) {
  debugPrint("[BLE]GPIO : set digital dir");
  pinMode(pin, (dir && 1) ? OUTPUT : INPUT);
}

void ioDigitalWrite(unsigned int pin, unsigned int data) {
  debugPrint("[BLE]GPIO : write digital value");
  digitalWrite(pin, data);
}

unsigned int ioDigitalRead(unsigned pin) {
  debugPrint("[BLE]GPIO : read digital value");
  return digitalRead(pin);
}

void ioAnalogWrite(int pin, int pwm) {
  debugPrint("[BLE]GPIO : write analog value");
  analogWrite(pin, pwm);
}

int ioAnalogRead(int pin) {
  int readValue = analogRead(pin);
  debugPrint("[BLE]GPIO : write analog value");
  return readValue;
}

/*********************************************************************************
* Setup
*********************************************************************************/
void setup() {
  // Serial init
  Serial.begin(115200);
  Bluefruit.autoConnLed(false);
  // Timer for display update
  timerDisplay.begin(500, displayUpdateEvent);
  // Timer for notify
  timerNotify.begin(800, bleNotifyEvent);
  // Timer for temperature notify interval (for NOTIFY_TEMP_CHARACTERISTIC_UUID)
  io_notify_temp_interval.begin(1000, bleIoNotifyTempEvent);
  // Disable LED control by bootloader
  Bluefruit.autoConnLed(false);
  // Switch port setting
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  // IO settnigs
  setupPin();
  // Display init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  // Acceleration sensor init
  accel.init(SCALE_2G);
  // Temperature sensor init
  temp.init();
  // ADC init
  analogReference(AR_VDD4);  // ADC reference = VDD
  analogReadResolution(10);  // ADC 10bit
  // Load advertising UUID config
  InternalFS.begin();
  // If SW1 is 1 when reset, or don't find config file in flash
  // Set initial service uuid in Flash memory.
  if (!digitalRead(SW1) || configFileExist() == -1) {
    uint8_t uuid128[16];
    strUUID2Bytes(DEFAULT_ADVERTISE_UUID, uuid128);
    configFileWrite(uuid128);
    String message = "Set advertising UUID to default.";
    debugPrint(message);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println(message);
    display.display();
    delay(5000);
  }
  // Read UUID config data
  configFileRead();
  // if UUID is default, TX power set to low
  uint8_t uuid128[16];
  strUUID2Bytes(DEFAULT_ADVERTISE_UUID, uuid128);
  int cmpUuid = compareUuid(uuid128, blesv_user_uuid);
  // BLE config
  if (cmpUuid == 0) {
    // Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
    bleConfigure(-40);
    debugPrint("[UUID]Using default advertising packet");
    debugPrint("[BLE]BLE transmitter power : -40dBm");
  } else {
    bleConfigure(0);
    debugPrint("BLE transmitter power : 0dBm");
  }
  bleSetupServicePsdi();
  bleSetupServiceDevice();
  if (memcmp(blesv_devboard_uuid, blesv_user_uuid, sizeof(blesv_devboard_uuid))) {
    bleSetupServiceUser();
  }
  bleStartAdvertising();
  // Set SW interrupt
  attachInterrupt(SW1, sw1ChangedEvent, CHANGE);
  attachInterrupt(SW2, sw2ChangedEvent, CHANGE);
}

void setupPin() {
  // Timer STOP
  io_notify_temp_interval.stop();
  // Set IO for LED
  pinMode(LED_DS2, OUTPUT);
  pinMode(LED_DS3, OUTPUT);
  pinMode(LED_DS4, OUTPUT);
  pinMode(LED_DS5, OUTPUT);
  digitalWrite(LED_DS2, 0);
  digitalWrite(LED_DS3, 0);
  digitalWrite(LED_DS4, 0);
  digitalWrite(LED_DS5, 0);
  // Set default value for GPIO
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  pinMode(GPIO5, OUTPUT);
  pinMode(GPIO12, OUTPUT);
  pinMode(GPIO13, OUTPUT);
  pinMode(GPIO14, OUTPUT);
  pinMode(GPIO15, OUTPUT);
  pinMode(GPIO16, OUTPUT);
  digitalWrite(GPIO2, 0);
  digitalWrite(GPIO3, 0);
  digitalWrite(GPIO4, 0);
  digitalWrite(GPIO5, 0);
  digitalWrite(GPIO12, 0);
  digitalWrite(GPIO13, 0);
  digitalWrite(GPIO14, 0);
  digitalWrite(GPIO15, 0);
  digitalWrite(GPIO16, 0);
  // Start Software Timer
  timerDisplay.start();
  timerNotify.start();
}

void command_characteristic_handler() {
  static byte i2cReadData = 0;
  unsigned int temp = 0;
  float accelData[3] = {0, 0, 0};
  // LED
  if (g_led.changed) {
    ledWrite(g_led.port, g_led.value);
    g_led.changed = 0;
  }
  // LED Byte
  if (g_led_byte.changed) {
    ledWrite(2, (g_led_byte.value >> 3) & 1);
    ledWrite(3, (g_led_byte.value >> 2) & 1);
    ledWrite(4, (g_led_byte.value >> 1) & 1);
    ledWrite(5, g_led_byte.value & 1);
    g_led_byte.changed = 0;
  }
  // Buzzer
  if (g_buzzer.changed) {
    buzzerWrite(g_buzzer.value);
    g_buzzer.changed = 0;
  }
  // GPIO Direction
  if (g_gpio_dir.changed) {
    ioDigitalDir(g_gpio_dir.port, g_gpio_dir.value);
    g_gpio_dir.changed = 0;
  }
  // GPIO Digital Write
  if (g_gpio_w_value.changed) {
    ioDigitalWrite(g_gpio_w_value.port, g_gpio_w_value.value);
    g_gpio_w_value.changed = 0;
  }
  // GPIO Analog write
  if (g_gpio_aw_value.changed) {
    ioAnalogWrite(g_gpio_aw_value.port, g_gpio_aw_value.value);
    g_gpio_aw_value.changed = 0;
  }
  // Display Control
  if (g_display.changed) {
    displaySetConfigure(g_display.addr_x, g_display.addr_y);
    g_display.changed = 0;
  }
  // Display Write text
  if (g_display_text.changed) {
    char text[16];
    for (int i = 0; i < 16; i++) {
      text[i] = g_display_text.text[i];
    }
    displayWrite(g_display_fontsize, g_display_text.length, text);
    g_display_text.changed = 0;
  }
  // Display Clear
  if (g_display_clear) {
    displayClear();
    g_display_clear = 0;
  }
  // I2C start Transmission
  if (g_i2c_start_transmission.changed) {
    i2cBeginTransmission(g_i2c_start_transmission.data);
    g_i2c_start_transmission.changed = 0;
    g_i2c_user_mode = 1;
  }
  // I2C write data
  if (g_i2c_w_value.changed) {
    i2cWrite(g_i2c_w_value.data);
    g_i2c_w_value.changed = 0;
  }
  // I2C stop transaction
  if (g_i2c_stop_transmission) {
    i2cEndTransmission();
    g_i2c_stop_transmission = 0;
    g_i2c_user_mode = 0;
  }
  // I2C request from
  if (g_i2c_request_from.changed) {
    i2cRequestFrom(g_i2c_request_from.data, g_i2c_request_from.length);
    g_i2c_request_from.changed = 0;
  }
  // I2C Read
  if (g_i2c_read_req) {
    g_i2c_read_req = 0;
    i2cReadData = i2cRead();
  }
  // Notify SW
  if (g_notify_sw.changed) {
    uint32_t mode;
    switch (g_notify_sw.mode) {
      case 0:
        mode = LOW;
        break;
      case 1:
        mode = CHANGE;
        break;
      case 2:
        mode = RISING;
        break;
      case 3:
        mode = FALLING;
        break;
      default:
        mode = LOW;
        break;
    }
    // Disable Switch callback
    detachInterrupt(SW1);
    detachInterrupt(SW2);
    // Check Switch interrupt
    if (g_notify_sw.source & 1) {
      attachInterrupt(SW1, sw1ChangedEvent, mode);
    }
    if (g_notify_sw.source & 2) {
      attachInterrupt(SW2, sw2ChangedEvent, mode);
    }
    g_notify_sw.changed = 0;
  }
  // Notify Temp
  if (g_notify_temp.changed) {
    if (g_notify_temp.source) {
      io_notify_temp_interval.setPeriod(g_notify_temp.interval);
      io_notify_temp_interval.start();
    } else {
      io_notify_temp_interval.stop();
    }
    g_notify_temp.changed = 0;
  }
  // BLE Read action
  if (g_read_action.changed) {
    byte data[4] = {0, 0, 0, 0};
    debugPrint("[BLE]Set read pointer : " + String(g_read_action.cmd));
    switch (g_read_action.cmd) {
      case 0:
        data[2] = swGetValue(2);
        data[3] = swGetValue(1);
        break;
      case 1:
        accelRead(accelData);
        data[1] = accelData[0];
        data[2] = accelData[1];
        data[3] = accelData[2];
        break;
      case 2:
        temp = tempRead() * 100;
        data[2] = temp >> 8;
        data[3] = temp & 0xff;
        break;
      case 3:
        data[3] = ioDigitalRead(g_gpio_r_port);
        break;
      case 4:
        data[3] = ioAnalogRead(g_gpio_ar_port);
        break;
      case 5:
        data[2] = 1;
        data[3] = i2cReadData;
        break;
      default:
        break;
    }
    // Set BLE Register
    blech_command_response.notify(data, sizeof(data));
    g_read_action.changed = 0;
  }
}

void loop() {
  // Change service UUID
  if (g_flag_update_advertiseuuid) {
    update_advertiseuuid();
  }

  // Set LED and buzzer value
  if (g_flag_user_write) {
    board_user_write();
    g_flag_user_write = 0;
  }

  bool refresh_display = g_flag_display && !g_display_user_mode && !g_i2c_user_mode;
  bool notify_sensor = g_flag_notify && blech_notify_board_state.notifyEnabled();
  
  float temperature;
  // Read sensor value
  if (!g_i2c_user_mode && (refresh_display || notify_sensor)) {
    temperature = temp.read();
    if (accel.available()) {
      accel.read();
    }
  }

  // Display write (Default)
  if (refresh_display) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    if (Bluefruit.connected()) {
      display.println("LINE Things [BLE]");
    } else {
      display.println("LINE Things");
    }
    display.print("X:");
    display.println(accel.cx);
    display.print("Y:");
    display.println(accel.cy);
    display.print("Z:");
    display.println(accel.cz);
    display.print("Temperature:");
    display.println(temperature);
    if (g_data_sw1) {
      display.print("SW1:ON / ");
    } else {
      display.print("SW1:OFF / ");
    }
    if (g_data_sw2) {
      display.println("SW2:ON");
    } else {
      display.println("SW2:OFF");
    }
    display.display();
    g_flag_display = 0;
  }

  if (Bluefruit.connected()) {
    // Notify board state
    if (notify_sensor) {
      notify_board_state(accel.cx, accel.cy, accel.cz, temperature, g_data_sw1, g_data_sw2);
      g_flag_notify = 0;
    }
    notify_sw_handler();
    notify_temp_handler();
    command_characteristic_handler();
  }
}

void update_advertiseuuid() {
  if (g_flag_error_advertiseuuid) {
    String errorMsg =
        "[ERROR] : Write new advertising UUID, Hash isn't match. "
        "Please retry it.";
    debugPrint(errorMsg);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println(errorMsg);
    display.display();
    delay(5000);
    g_flag_update_advertiseuuid = 0;
    g_flag_error_advertiseuuid = 0;
    return;
  }
  configFileWrite(blesv_user_uuid);
  configFileRead();
  debugPrint("BLE advertising uuid changed from LIFF.");
  debugPrint("Enable new uuid after restart MPU.");
  debugPrint("Please push reset button.");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("BLE advertising uuid changed from LIFF.");
  display.println("Enable new uuid after restart MPU.");
  display.println("Please push reset button.");
  display.display();
  for (;;);
}

void board_user_write() {
  // Set GPIO output mode
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  pinMode(GPIO5, OUTPUT);
  pinMode(GPIO12, OUTPUT);
  pinMode(GPIO13, OUTPUT);
  pinMode(GPIO14, OUTPUT);
  pinMode(GPIO15, OUTPUT);
  pinMode(GPIO16, OUTPUT);
  // Buzzer
  if (g_data_user_write_buzzer) {
    buzzerStart();
  } else {
    buzzerStop();
  }
  // LED
  digitalWrite(LED_DS2, g_data_user_write_led0);
  digitalWrite(LED_DS3, g_data_user_write_led1);
  digitalWrite(LED_DS4, g_data_user_write_led2);
  digitalWrite(LED_DS5, g_data_user_write_led3);
  // GPIO
  digitalWrite(GPIO2, g_data_user_write_io2);
  digitalWrite(GPIO3, g_data_user_write_io3);
  digitalWrite(GPIO4, g_data_user_write_io4);
  digitalWrite(GPIO5, g_data_user_write_io5);
  digitalWrite(GPIO12, g_data_user_write_io12);
  digitalWrite(GPIO13, g_data_user_write_io13);
  digitalWrite(GPIO14, g_data_user_write_io14);
  digitalWrite(GPIO15, g_data_user_write_io15);
  digitalWrite(GPIO16, g_data_user_write_io16);
}

void notify_sw_handler() {
  if (blech_notify_sw.notifyEnabled()) {
    if (g_flag_sw1 && (millis() - g_last_notified_sw1) > g_notify_sw.interval) {
      byte notify[2] = {1, g_data_sw1};  // {SW, value}
      g_flag_sw1 = 0;
      g_last_notified_sw1 = millis();
      debugPrint("SW1 Event");
      blech_notify_sw.notify(notify, sizeof(notify));
    }
    if (g_flag_sw2 && (millis() - g_last_notified_sw2) > g_notify_sw.interval) {
      byte notify[2] = {2, g_data_sw2};  // {SW, value}
      g_flag_sw2 = 0;
      g_last_notified_sw2 = millis();
      debugPrint("SW2 Event");
      blech_notify_sw.notify(notify, sizeof(notify));
    }
  }
}

void notify_temp_handler() {
  if (g_i2c_user_mode) {
    debugPrint("User using I2C from JS. Do not work temperature notify when user using I2C from JS");
    return;
  }
  if (g_flag_io_temp_read && blech_notify_temp.notifyEnabled()) {
    unsigned int temp = tempRead() * 100;
    byte notify[2] = {temp >> 8, temp & 0xff};
    debugPrint("TEMP Event");
    blech_notify_temp.notify(notify, sizeof(notify));
    g_flag_io_temp_read = 0;
  }
}

void notify_board_state(float acc_x, float acc_y, float acc_z, float temperature, int sw1, int sw2) {
  int16_t tx_frame[8] = {
    (int16_t) temperature * 100,
    acc_x * 1000,
    acc_y * 1000,
    acc_z * 1000,
    sw1,
    sw2,
    FIRMWARE_VERSION,
    0
  };
  blech_notify_board_state.notify((uint8_t*)tx_frame, sizeof(tx_frame));
}
