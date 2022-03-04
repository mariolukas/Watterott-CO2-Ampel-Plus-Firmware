#ifndef SENSOR_H
#define SENSOR_H
#include <Arduino.h>
#include "MQTTManager.h"

void sensor_handler(bool visualize_via_leds);
void sensor_init();
void sensor_handle_brightness();
void sensor_set_temperature_offset(float offset);
bool sensor_get_co2_autocalibration();
void sensor_set_co2_autocalibration(bool enable);
void sensor_allow_co2_force_recalibration(bool allow);
void sensor_do_co2_force_recalibration(uint32_t current_accurately_measured_co2_value);

float get_temperature();
unsigned int get_co2();
float get_humidity();
unsigned int get_brightness();

#endif
