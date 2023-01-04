#ifndef CO2SENSOR_H
#define CO2SENSOR_H
#include <Arduino.h>
#include "MQTTManager.h"
#include "scheduler.h"

extern Task task_init_co2_sensor;
constexpr uint32_t INIT_CO2_SENSOR_TASK_PERIOD_MS = 100;
constexpr uint32_t INIT_CO2_SENSOR_MAX_TRIES = 3;

extern Task task_read_co2_sensor;
constexpr uint32_t CO2_SENSOR_TASK_PERIOD_MS = 1000;
constexpr uint32_t CO2_SENSOR_MEASUREMENT_COUNT = 2;

struct co2_sensor_measurement_t {
  uint16_t co2;
  float temperature;
  float humidity;

  co2_sensor_measurement_t& operator+=(const co2_sensor_measurement_t& rhs) {
    this->co2 += rhs.co2;
    this->temperature += rhs.temperature;
    this->humidity += rhs.humidity;
    return *this;
  }

  co2_sensor_measurement_t operator/(uint16_t divisor) {
    return {
        static_cast<uint16_t>(static_cast<uint32_t>(co2) /
                              static_cast<uint32_t>(divisor)),
        temperature / divisor,
        humidity / divisor,
    };
  };
};

void init_co2_sensor(Scheduler& scheduler);
void sensor_calibration();
void sensor_set_temperature_offset(float offset);

bool get_co2_sensor_measurement(co2_sensor_measurement_t& measurement);

#endif
