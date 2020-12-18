#include <Arduino.h>
#include "Buzzer.h"
#include "Config.h"
#include "DeviceConfig.h"

void buzzer_init() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

void buzzer_test() {
  analogWrite(PIN_BUZZER, BUZZER_VOLUME);  // Buzzer on
  delay(1000);
  analogWrite(PIN_BUZZER, 0);  // Buzzer off
}

void buzzer_ack() {
  analogWrite(PIN_BUZZER, BUZZER_VOLUME);  // Buzzer on
  delay(500);
  analogWrite(PIN_BUZZER, 0);  // Buzzer off
}

void buzzer_on() {
  device_config_t cfg = config_get_values();
  if (cfg.buzzer_enabled) {
    analogWrite(PIN_BUZZER, BUZZER_VOLUME);  // Buzzer on
  }
}

void buzzer_off() {
  analogWrite(PIN_BUZZER, 0);  // Buzzer off
}
