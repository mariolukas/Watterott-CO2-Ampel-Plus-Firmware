#include <Arduino.h>
#include "Config.h"
#include "LED.h"

constexpr uint32_t LED_COLOR_SENSOR_FAILURE = LED_VIOLET;

void led_wifi_failure() {
  led_state_queue.push(led_state_t{
      set_leds_alternate, 250, 20 * 250,
      std::vector<uint32_t>{LED_COLOR_WIFI_FAILURE, LED_COLOR_WIFI_FAILURE, 0,
                            0, 0, 0, LED_COLOR_WIFI_FAILURE,
                            LED_COLOR_WIFI_FAILURE},
      0});
}

void led_wifi_connecting() {
  led_state_queue.push(led_state_t{
      set_leds_alternate, 10, 50 * 10,
      std::vector<uint32_t>{
          LED_COLOR_WIFI_CONNECTING, LED_COLOR_WIFI_CONNECTING, 0, 0, 0, 0,
          LED_COLOR_WIFI_CONNECTING, LED_COLOR_WIFI_CONNECTING},
      0});
}

void led_wifi_connected() {
  led_state_queue.push(
      led_state_t{set_leds_on, 100, 1 * 100,
                  std::vector<uint32_t>{LED_COLOR_WIFI_CONNECTING}, 0});
}

void led_sensor_failure() {
  led_state_queue.push(led_state_t{
      set_leds_alternate, 250, 20 * 250,
      std::vector<uint32_t>{LED_COLOR_SENSOR_FAILURE, LED_COLOR_SENSOR_FAILURE,
                            0, 0, 0, 0, LED_COLOR_SENSOR_FAILURE,
                            LED_COLOR_SENSOR_FAILURE},
      0});
}

void led_on_color(uint32_t color) {
  led_state_queue.push(
      led_state_t{set_leds_on, 250, 20 * 250, std::vector<uint32_t>{color}, 0});
}

void led_default_blink(uint32_t color) {
  led_set_default({
      set_leds_blink,
      200,
      2 * 200,
      std::vector<uint32_t>{color},
  });
}

void led_blink_color(uint32_t color) {
  led_state_queue.push(led_state_t{set_leds_blink, 250, 20 * 250,
                                   std::vector<uint32_t>{color}, 0});
}

void led_default_on(uint32_t color) {
  led_set_default({
      set_leds_on,
      200,
      -1,
      std::vector<uint32_t>{color},
  });
}

void led_off() {
  led_queue_flush();
}