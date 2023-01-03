#include "LightSensor.h"
#include <CircularBuffer.h>
#include <array>
#include "LED.h"

uint16_t ambient_brightness = 0;

uint16_t get_ambient_brightness() {
  return ambient_brightness;
}

void init_light_sensor() {
  pinMode(PIN_LSENSOR_PWR, OUTPUT);
  digitalWrite(PIN_LSENSOR_PWR, LOW);
  pinMode(PIN_LSENSOR, INPUT);
}

void read_light_sensor();
Task task_read_light_sensor(  //
    LIGHT_SENSOR_TASK_PERIOD_MS* TASK_MILLISECOND,
    -1,
    read_light_sensor,
    &ts);

void read_light_sensor() {
  static LIGHT_SENSOR_STATES state = LIGHT_SENSOR_STATES::PRE_MEASURING;
  static CircularBuffer<uint16_t, LIGHT_SENSOR_MEASUREMENT_COUNT> measurements;

  switch (state) {
    case LIGHT_SENSOR_STATES::PRE_MEASURING:
      if (BLANK_LEDS_DURING_MEASUREMENT)
        led_blank();
      digitalWrite(PIN_LSENSOR_PWR, HIGH);
      state = LIGHT_SENSOR_STATES::MEASURING;
      task_read_light_sensor.delay(LIGHT_SENSOR_PRE_BLANKING_MS);
      break;

    case LIGHT_SENSOR_STATES::MEASURING:
      if (!measurements.isFull()) {
        measurements.push(analogRead(PIN_LSENSOR));
        task_read_light_sensor.delay(LIGHT_SENSOR_READ_PERIOD_MS);
      }

      if (measurements.isFull()) {
        uint32_t sum = 0;
        uint32_t measurement_count = 0;

        while (!measurements.isEmpty()) {
          sum += measurements.shift();
          measurement_count++;
        }
        ambient_brightness = sum / measurement_count;

        state = LIGHT_SENSOR_STATES::POST_MEASURING;
        digitalWrite(PIN_LSENSOR_PWR, LOW);
        task_read_light_sensor.delay(LIGHT_SENSOR_POST_BLANKING_MS);
      }
      break;

    case LIGHT_SENSOR_STATES::POST_MEASURING:
      if (BLANK_LEDS_DURING_MEASUREMENT)
        led_unblank();
      state = LIGHT_SENSOR_STATES::PRE_MEASURING;
      task_read_light_sensor.disable();
  }
}

void trigger_read_light_sensor();
Task task_trigger_read_light_sensor(  //
    LIGHT_SENSOR_MEASUREMENT_PERIOD_MS* TASK_MILLISECOND,
    -1,
    trigger_read_light_sensor,
    &ts);

void trigger_read_light_sensor() {
  task_read_light_sensor.enableIfNot();
}
