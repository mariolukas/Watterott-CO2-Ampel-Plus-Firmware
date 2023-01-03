#include "LED.h"
#include <Adafruit_NeoPixel.h>
#include <queue>
#include <vector>
#include "Buzzer.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "LightSensor.h"
#include "NetworkManager.h"

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(NUMBER_OF_WS2312_PIXELS,
                                             PIN_WS2812,
                                             NEO_GRB + NEO_KHZ800);
byte led_brightness = BRIGHTNESS;

std::vector<uint32_t> BLACK_VECT = {0};

std::queue<led_state_t> led_state_queue;
bool blank_requested = false;
bool blanked = false;

void led_blank() {
  blank_requested = true;
}

void led_unblank() {
  blank_requested = false;
};

bool fill_all(std::vector<uint32_t>& colors) {
  bool update_required = false;
  for (int i = 0; i < NUMBER_OF_WS2312_PIXELS; i++) {
    // TODO: This runs everytime if we have adjusted brightness
    // because of the scaling done in WS2812 library.
    // Implement a function to get the raw color and compare it
    // to a brightness scaled target color. This applies everywhere
    // where we getPixelColor...
    auto current_color = ws2812.getPixelColor(i);
    auto color = colors[i % colors.size()];
    if (current_color != color) {
      ws2812.setPixelColor(i, color);
      update_required = true;
    }
  }
  return update_required;
}

bool fill_circle(std::vector<uint32_t>& colors,
                 int run_count,
                 bool ccw = false) {
  bool update_required = false;
  auto color_index = (run_count / NUMBER_OF_WS2312_PIXELS) % colors.size();
  auto color = colors[color_index];
  auto pixel_index = run_count % NUMBER_OF_WS2312_PIXELS;

  if (ccw == true) {
    pixel_index = NUMBER_OF_WS2312_PIXELS - pixel_index - 1;
  }

  if (ws2812.getPixelColor(pixel_index) != color) {
    ws2812.setPixelColor(pixel_index, color);
    update_required = true;
  }
  return update_required;
}

std::vector<uint32_t> failure_colors(std::vector<uint32_t>& colors) {
  size_t const half_size = colors.size() / 2;
  auto failure_colors =
      std::vector<uint32_t>(colors.begin() + half_size, colors.end());
  failure_colors.insert(failure_colors.end(), colors.begin(),
                        colors.begin() + half_size);
  return failure_colors;
}

bool set_leds_off(led_state_t& led_state, uint32_t run_time_ms) {
  return fill_all(BLACK_VECT);
}

bool set_leds_on(led_state_t& led_state, uint32_t run_time_ms) {
  return fill_all(led_state.colors);
}

bool set_leds_blink(led_state_t& led_state, uint32_t run_time_ms) {
  bool update_required = false;
  if ((run_time_ms / led_state.period_ms) % 2 == 0)
    update_required = fill_all(led_state.colors);
  else {
    update_required = fill_all(BLACK_VECT);
  }
  return update_required;
}

bool set_leds_circle_cw(led_state_t& led_state, uint32_t run_time_ms) {
  int run_count = run_time_ms / led_state.period_ms;
  return fill_circle(led_state.colors, run_count, false);
}

bool set_leds_circle_ccw(led_state_t& led_state, uint32_t run_time_ms) {
  int run_count = run_time_ms / led_state.period_ms;
  return fill_circle(led_state.colors, run_count, true);
}

bool set_leds_alternate(led_state_t& led_state, uint32_t run_time_ms) {
  const size_t middle = led_state.colors.size() / 2;
  std::vector<uint32_t> colors_a;
  std::vector<uint32_t> colors_b;

  if (middle == 1) {
    colors_a = {led_state.colors[0]};
    colors_b = {led_state.colors[1]};
  } else {
    colors_a = std::vector<uint32_t>(led_state.colors.cbegin(),
                                     led_state.colors.cbegin() + middle);
    colors_b = std::vector<uint32_t>(led_state.colors.cbegin() + middle,
                                     led_state.colors.cend());
  }

  bool update_required = false;
  int run_count = run_time_ms / led_state.period_ms;
  if (run_count % 2 == 0)
    update_required = fill_all(colors_a);
  else {
    update_required = fill_all(colors_b);
  }
  return update_required;
}

void led();
Task task_led(LED_DEFAULT_PERIOD_MS* TASK_MILLISECOND, -1, led, &ts);

led_state_t led_default_state = {
    set_leds_off, LED_DEFAULT_PERIOD_MS, 0, std::vector<uint32_t>{0}, 0,
};

bool led_queue_done() {
  return led_state_queue.empty();
}

led_state_t get_led_state() {
  if (!led_state_queue.empty()) {
    return led_state_queue.front();
  }
  // fall back to default state if nothing is queued
  return led_default_state;
}

void led_queue_flush() {
  std::queue<led_state_t>().swap(led_state_queue);
}

int8_t led_brightness_step(uint8_t target_brightness) {
  float step =
      float(target_brightness - led_brightness) * LED_BRIGHTNESS_FADE_FACTOR;

  if (step > 0 && step < 1)
    step = 1;
  else if (step < 0 && step > -1)
    step = -1;
  else
    step = static_cast<int8_t>(roundl(step));

  return step;
}

void led() {
  bool update_required = false;
  static int32_t current_state_run_count = 0;
  led_state_t led_state = get_led_state();

  current_state_run_count++;
  int32_t current_state_run_time_ms =
      current_state_run_count * LED_DEFAULT_PERIOD_MS;

  if (led_state.run_time_ms != -1 &&
      current_state_run_time_ms >= led_state.run_time_ms) {
    // Current state is done
    if (!led_state_queue.empty()) {
      led_state_queue.pop();
    }
    current_state_run_count = 0;
    led_state = get_led_state();
  }

  if ((led_state.run_time_ms == -1) ||
      (current_state_run_time_ms - led_state._last_run_time_ms >=
       led_state.period_ms)) {
    update_required = led_state.handler(led_state, current_state_run_time_ms);
    led_state._last_run_time_ms = current_state_run_time_ms;
  }

  auto cfg = config_get_values();
  if (cfg.light_enabled == false) {
    ws2812.clear();
    update_required = true;
  }

  if (cfg.led_brightness == -1) {
    auto auto_brightness = static_cast<uint8_t>(
        lroundf(static_cast<float>(get_ambient_brightness()) /
                static_cast<float>(LIGHT_SENSOR_MAX_BRIGHTNESS) * 255.0f));
    led_brightness += led_brightness_step(auto_brightness);
    update_required = true;
  } else if (led_brightness != cfg.led_brightness) {
    led_brightness += led_brightness_step(cfg.led_brightness);
    update_required = true;
  }

  if (blank_requested == true && blanked == false) {
    ws2812.clear();
    blanked = true;
    update_required = true;
  } else if (blank_requested == false && blanked == true) {
    ws2812.setBrightness(led_brightness);
    blanked = false;
    update_required = true;
  }

  if (update_required == true) {
    ws2812.setBrightness(led_brightness);
    ws2812.show();
  }
}

void run_until_queue_size(uint32_t stop_queue_size) {
  auto queue_size = led_state_queue.size();
  do {
    ts.execute();
    queue_size = led_state_queue.size();
  } while (queue_size > stop_queue_size);
}

void led_set_default(led_state_t led_state) {
  led_default_state = led_state;
}

void init_leds() {
#if DEBUG_LOG > 0
  Serial.print("Initialise LEDs... ");
#endif
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);  // LED aus
  pinMode(PIN_WS2812, OUTPUT);
  digitalWrite(PIN_WS2812, LOW);
  ws2812.clear();
  ws2812.show();
  led_queue_flush();
#if DEBUG_LOG > 0
  Serial.println("done!");
#endif
}

// OLD STUBS
uint32_t led_get_color() {
  return 0;
}
