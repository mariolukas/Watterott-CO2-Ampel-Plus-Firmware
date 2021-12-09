#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include <queue>
#include <vector>
#include "scheduler.h"

constexpr uint32_t BUZZER_DEFAULT_PERIOD_MS = 10;
constexpr float BUZZER_VOLUME_FADE_FACTOR = 0.1;

extern Task task_buzzer;

typedef struct buzzer_state_t {
  uint8_t (*handler)(buzzer_state_t&, uint32_t);
  uint32_t period_ms;
  int32_t run_time_ms;
  std::vector<uint8_t> volumes;
  uint32_t _last_run_time_ms;
} buzzer_state_t;

extern std::queue<buzzer_state_t> buzzer_state_queue;

void init_buzzer(Scheduler& scheduler);
void old_test_buzzer();
void test_buzzer();

void buzzer_queue_flush();
void buzzer_set_default(buzzer_state_t buzzer_state);

void buzzer_mute();
void buzzer_unmute();

#endif
