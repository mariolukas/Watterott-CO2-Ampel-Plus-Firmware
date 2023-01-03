#ifndef LED_H
#define LED_H
#include <Arduino.h>
#include <queue>
#include <vector>
#include "scheduler.h"

void init_leds();

uint32_t led_get_color();
void led_adjust_brightness(uint32_t);

constexpr uint32_t LED_DEFAULT_PERIOD_MS = 10;
constexpr float LED_BRIGHTNESS_FADE_FACTOR = 0.1;

extern Task task_led;

typedef struct led_state_t {
  bool (*handler)(led_state_t&, uint32_t);
  uint32_t period_ms;
  int32_t run_time_ms;
  std::vector<uint32_t> colors;
  uint32_t _last_run_time_ms;
} led_state_t;

extern std::queue<led_state_t> led_state_queue;

bool set_leds_on(led_state_t& led_state, uint32_t run_time_ms);
bool set_leds_blink(led_state_t& led_state, uint32_t run_time_ms);
bool set_leds_circle_cw(led_state_t& led_state, uint32_t run_time_ms);
bool set_leds_circle_ccw(led_state_t& led_state, uint32_t run_time_ms);
bool set_leds_failure(led_state_t& led_state, uint32_t run_time_ms);
bool set_leds_alternate(led_state_t& led_state, uint32_t run_time_ms);

void led_queue_flush();
void run_until_queue_size(uint32_t stop_queue_size);
void led_set_default(led_state_t led_state);

void led_blank();
void led_unblank();

#endif
