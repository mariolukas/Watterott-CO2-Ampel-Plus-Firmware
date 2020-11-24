#ifndef SENSOR_H
#define SENSOR_H
#include <Arduino.h>
#include "MQTTManager.h"

void sensor_handler();
void sensor_init();
void sensor_calibration();
void sensor_handle_brightness();
void sensor_set_temperature_offset(float offset);

float get_temperature();
unsigned int get_co2();
float get_humidity();
unsigned int get_brightness();

#endif
