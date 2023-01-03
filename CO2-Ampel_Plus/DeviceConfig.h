#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H
#include <FlashStorage.h>

typedef struct {
  int change_count;
  char wifi_ssid[40];
  char wifi_password[64];
  char ap_password[64];
  int mqtt_broker_port;
  char mqtt_broker_address[32];
  char mqtt_topic[32];
  char ampel_name[32];
  float temperature_offset;
  char mqtt_username[32];
  char mqtt_password[64];
  int mqtt_format;
  bool light_enabled;
  bool buzzer_enabled;
  int led_brightness;
} device_config_t;

void config_set_factory_defaults();
bool config_is_initialized();
device_config_t config_get_values();
void config_set_values(device_config_t new_config);

#endif
