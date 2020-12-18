#include <Adafruit_NeoPixel.h>
#include "Led.h"
#include "Buzzer.h"
#include "Config.h"

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(NUMBER_OF_WS2312_PIXELS,
                                             PIN_WS2812,
                                             NEO_GRB + NEO_KHZ800);
byte connecting_tick = 0;
byte led_tick = 0;
byte led_brightness = BRIGHTNESS;

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
  ws2812.show();
  delay(500);
  ws2812.setPixelColor(0, ws2812.Color(0, 0, 0));
  ws2812.setPixelColor(1, ws2812.Color(0, 0, 0));
  ws2812.setPixelColor(2, color);
  ws2812.setPixelColor(3, color);
  ws2812.show();
  delay(500);
}

void led_test() {
  fill_led_by_led(LED_RED);
  fill_led_by_led(LED_YELLOW);
  fill_led_by_led(LED_GREEN);
  fill_led_by_led(LED_BLUE);
  analogWrite(PIN_BUZZER, 0);
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
}

void led_set_brightness() {
  ws2812.setBrightness(led_brightness);
}

void led_adjust_brightness(byte brightness) {
  led_brightness = brightness;
}

void led_blink(uint32_t color, int intervall) {
  ws2812.fill(ws2812.Color(0, 0, 0), 0, 4);
  // led_set_brightness(led_brightness);
  buzzer_on();  // Buzzer aus
  ws2812.show();
  delay(intervall);
  ws2812.fill(color, 0, 4);  // rot maximale Helligkeit
  // led_adjust_brigh(led_brightness);
  buzzer_off();
  ws2812.show();
  delay(intervall);
}

uint32_t led_get_color() {
  return ws2812.getPixelColor(0);
}

void led_update() {
  // led_set_brightness_to(BRIGHTNESS);
  ws2812.show();
}

void led_wifi_ap_mode() {
  // fade in
  for (int x = BRIGHTNESS_DARK; x < BRIGHTNESS; x++) {
    delay(10);
    led_adjust_brightness(x);
    led_set_brightness();
  }
  // fade out
  for (int x = BRIGHTNESS; x >= BRIGHTNESS_DARK; x--) {
    delay(10);
    led_adjust_brightness(x);
    led_set_brightness();
  }
}

/*
void led_pulse(){
    bool fadeup=true, fadedown=false;
    if(led_tick <= 255 && fadeup){
      fadeup = true;
      if (tick == 255){
         fadedown = true;
      }  else {
          tick++;
      }
    }

    if (led_tick >= 255 && fadedown){
      fadedown = true;
      if (tick == 0){
         fadeup = true;
      }  else {
          tick--;
      }
    }
    led_set_brightness(led_tick);
    delay(10);
}
*/

void led_init() {
  Serial.println("Initialise LEDs");
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);  // LED aus
  pinMode(PIN_WS2812, OUTPUT);
  digitalWrite(PIN_WS2812, LOW);
  ws2812.clear();
  ws2812.show();
}
