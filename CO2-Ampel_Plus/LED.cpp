#include <Adafruit_NeoPixel.h>
#include "LED.h"
#include "Buzzer.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "NetworkManager.h"

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(NUMBER_OF_WS2312_PIXELS,
                                             PIN_WS2812,
                                             NEO_GRB + NEO_KHZ800);
byte connecting_tick = 0;
byte led_tick = 0;
bool led_darkmode = 0;

void fill_led_by_led(uint32_t color) {
  for (int i = 0; i < 4; i++) {
    ws2812.setPixelColor(i, color);
    ws2812.show();
    delay(100);
  }
}

void led_failure(uint32_t color) {

  ws2812.setPixelColor(0, color);
  ws2812.setPixelColor(1, color);
  ws2812.setPixelColor(2, ws2812.Color(0, 0, 0));
  ws2812.setPixelColor(3, ws2812.Color(0, 0, 0));
  ws2812.setBrightness(100);

  ws2812.show();
  delay(500);
  ws2812.setPixelColor(0, ws2812.Color(0, 0, 0));
  ws2812.setPixelColor(1, ws2812.Color(0, 0, 0));

  ws2812.setBrightness(100);
  ws2812.setPixelColor(2, color);
  ws2812.setPixelColor(3, color);

  ws2812.show();
  delay(500);
}

void led_test() {
#if DEBUG_LOG > 0
  Serial.print("LED selftest... ");
#endif
  fill_led_by_led(LED_RED);
  fill_led_by_led(LED_YELLOW);
  fill_led_by_led(LED_GREEN);
  fill_led_by_led(LED_BLUE);
#if DEBUG_LOG > 0
  Serial.println("done!");
#endif
}

void led_one_by_one(uint32_t color, int interval) {
  ws2812.clear();
  ws2812.setPixelColor(led_tick, color);
  if (led_tick == NUMBER_OF_WS2312_PIXELS) {
    led_tick = 0;
  } else {
    led_tick++;
  }
  delay(interval);
}

void led_broker_connection_fail() {
  delay(100);
  led_set_color(LED_WHITE);
  led_update();
  delay(100);
  led_off();
}

void led_ack() {
  for (byte i = 0; i < 2; i++) {
    ws2812.fill(ws2812.Color(0, 0, 0), 0, NUMBER_OF_WS2812_LEDS);
    ws2812.setBrightness(255);
    led_update();
    delay(200);
    led_off();
  }
}

void led_off() {
  ws2812.fill(ws2812.Color(0, 0, 0), 0, NUMBER_OF_WS2812_LEDS);  // LEDs aus
  analogWrite(PIN_BUZZER, 0);
  ws2812.show();
}

void led_set_color(uint32_t color) {
  ws2812.fill(color, 0, NUMBER_OF_WS2812_LEDS);  // LEDs aus
  led_set_brightness();
  ws2812.show();
}

void led_set_brightness() {
  device_config_t cfg = config_get_values();
  if (cfg.led_brightness > 0 || ap_is_active()) {

    if (led_darkmode == 1) {
      ws2812.setBrightness(255 / (100 / BRIGHTNESS_DARK));
    } else {
      ws2812.setBrightness(255 / (100 / cfg.led_brightness));
    }
  } else {
    ws2812.setBrightness(0);
  }
}

void led_set_darkmode(bool darkmode) {
  led_darkmode = darkmode;
}

void led_blink(uint32_t color, int intervall) {

  ws2812.fill(ws2812.Color(0, 0, 0), 0, 4);
  buzzer_on();  // Buzzer aus
  ws2812.show();
  delay(intervall);
  ws2812.fill(color, 0, 4);  // rot maximale Helligkeit
  buzzer_off();
  ws2812.show();
  delay(intervall);
}

uint32_t led_get_color() {
  return ws2812.getPixelColor(0);
}

void led_update() {
  led_set_brightness();
  ws2812.show();
}

void led_init() {
#if DEBUG_LOG > 0
  Serial.print("Initialise LEDs... ");
#endif
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);  // LED aus
  pinMode(PIN_WS2812, OUTPUT);
  digitalWrite(PIN_WS2812, LOW);
  ws2812.clear();
  ws2812.show();
#if DEBUG_LOG > 0
  Serial.println("done!");
#endif
}
