#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIXELS 16
#define NEOPIXEL_PIN 2

#define SW1 29

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEOPIXEL_PIXELS, NEOPIXEL_PIN, (NEO_RGB + NEO_KHZ800));

volatile int sw_disable_flag = 0;
SoftwareTimer chat_timer;
void clearChattering(TimerHandle_t xTimerID) {
  sw_disable_flag = 0;
  chat_timer.stop();
}

volatile int color; // 0:Red, 1:Green, 2:Blue, 3:White

void sw1Pushed() {
  if (!sw_disable_flag) {
    color++;
    if (color > 3) {
      color = 0;
    }
    sw_disable_flag = 1;
    chat_timer.start();
  }
}

void setup() {
  //Neopixelの初期化
  pixels.begin();
  delay(100);
  pixels.show();

  //チャタリング対策のタイマを設定(100mS)
  chat_timer.begin(100, clearChattering);

  //SW1を割り込みピンに指定
  pinMode(SW1, INPUT_PULLUP);
  attachInterrupt(SW1, sw1Pushed, FALLING);

}

void loop() {
  for(int i = 0; i < NEOPIXEL_PIXELS; i++) {
    for(int j = 0; j < 256; j += 4) {
      switch(color) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0, 0, j));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(0, j, 0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(j, 0, 0));
          break;
        default:
          pixels.setPixelColor(i, pixels.Color(j, j, j));
          break;
      }
      pixels.show();
      delay(10);
    }
  }
}
