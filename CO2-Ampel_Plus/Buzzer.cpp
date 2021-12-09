#include "Buzzer.h"
#include <queue>
#include <vector>
#include "Config.h"
#include "DeviceConfig.h"

std::queue<buzzer_state_t> buzzer_state_queue;
bool mute_requested = false;
bool muted = false;

uint8_t buzzer_volume = BUZZER_VOLUME;

void old_test_buzzer() {
  analogWrite(PIN_BUZZER, BUZZER_VOLUME);
  delay(1000);
  analogWrite(PIN_BUZZER, 0);
}

void buzzer_mute() {
  mute_requested = true;
}

void buzzer_unmute() {
  mute_requested = false;
};

uint8_t set_buzzer_on(buzzer_state_t& buzzer_state, uint32_t run_time_ms) {
  return *buzzer_state.volumes.cbegin();
}

uint8_t set_buzzer_off(buzzer_state_t& buzzer_state, uint32_t run_time_ms) {
  return 0;
}

uint8_t set_buzzer_alternating(buzzer_state_t& buzzer_state,
                               uint32_t run_time_ms) {
  if ((run_time_ms / buzzer_state.period_ms) % 2 == 0)
    return *buzzer_state.volumes.cbegin();
  else {
    return *buzzer_state.volumes.cend();
  }
}

buzzer_state_t buzzer_default_state = {
    set_buzzer_off, BUZZER_DEFAULT_PERIOD_MS, 0, std::vector<uint8_t>{0}, 0,
};

bool buzzer_queue_done() {
  return buzzer_state_queue.empty();
}

buzzer_state_t get_buzzer_state() {
  if (!buzzer_state_queue.empty()) {
    return buzzer_state_queue.front();
  }
  // fall back to default state if nothing is queued
  return buzzer_default_state;
}

void buzzer_queue_flush() {
  std::queue<buzzer_state_t>().swap(buzzer_state_queue);
}

int8_t buzzer_volume_step(uint8_t target_volume) {
  float step = float(target_volume - buzzer_volume) * BUZZER_VOLUME_FADE_FACTOR;

  if (step > 0 && step < 1)
    step = 1;
  else if (step < 0 && step > -1)
    step = -1;
  else
    step = static_cast<int8_t>(roundl(step));

  return step;
}

void buzzer();
Task task_buzzer(BUZZER_DEFAULT_PERIOD_MS* TASK_MILLISECOND, -1, buzzer);

void init_buzzer(Scheduler& scheduler) {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  scheduler.addTask(task_buzzer);
  task_buzzer.enable();
}

void buzzer() {
  static int32_t current_state_run_count = 0;
  buzzer_state_t buzzer_state = get_buzzer_state();
  uint8_t requested_volume = buzzer_volume;

  current_state_run_count++;
  int32_t current_state_run_time_ms =
      current_state_run_count * BUZZER_DEFAULT_PERIOD_MS;

  if (buzzer_state.run_time_ms != -1 &&
      current_state_run_time_ms >= buzzer_state.run_time_ms) {
    // Current state is done
    if (!buzzer_state_queue.empty()) {
      buzzer_state_queue.pop();
    }
    current_state_run_count = 0;
    buzzer_state = get_buzzer_state();
  }

  if ((buzzer_state.run_time_ms == -1) ||
      (current_state_run_time_ms - buzzer_state._last_run_time_ms >=
       buzzer_state.period_ms)) {
    requested_volume =
        buzzer_state.handler(buzzer_state, current_state_run_time_ms);
    requested_volume = buzzer_volume + buzzer_volume_step(requested_volume);
    buzzer_state._last_run_time_ms = current_state_run_time_ms;
  }

  auto cfg = config_get_values();
  if (cfg.buzzer_enabled == false) {
    analogWrite(PIN_BUZZER, 0);
    requested_volume = 0;
    return;
  }

  if (mute_requested == true && muted == false) {
    requested_volume = 0;
    muted = true;
  } else if (mute_requested == false && muted == true) {
    muted = false;
  }

  analogWrite(PIN_BUZZER, requested_volume);
  buzzer_volume = requested_volume;
}

/*
// TODO: Implement this more generically
void run_until_queue_size(uint32_t stop_queue_size) {
  auto queue_size = buzzer_state_queue.size();
  do {
    ts.execute();
    queue_size = buzzer_state_queue.size();
  } while (queue_size > stop_queue_size);
}
*/

void buzzer_set_default(buzzer_state_t buzzer_state) {
  buzzer_default_state = buzzer_state;
}

void test_buzzer() {
  buzzer_queue_flush();
  buzzer_state_queue.push(buzzer_state_t{set_buzzer_alternating, 80, 20 * 80,
                                         std::vector<uint8_t>{255, 0}, 0});
}
