#include "DeviceConfig.h"
#include "Config.h"
#include "LED.h"
FlashStorage(config_store, device_config_t);

bool config_is_initialized() {
  device_config_t _device_config;
  _device_config = config_store.read();
  if (_device_config.change_count > 0) {
    return true;
  } else {
    return false;
  }
}

void config_set_factory_defaults() {
  device_config_t _default_config = {
      1,
      WIFI_WPA_SSID,
      WIFI_WPA_PASSWORD,
      WIFI_AP_PASSWORD,
      MQTT_BROKER_PORT,
      MQTT_BROKER_ADDR,
      MQTT_TOPIC,
      MQTT_DEVICE_NAME,
      TEMPERATURE_OFFSET,
      MQTT_USERNAME,
      MQTT_PASSWORD,
      MQTT_FORMAT,
      LIGHT_ENABLED,
      BUZZER_ENABLED,
      BRIGHTNESS,
  };
  config_store.write(_default_config);
}

void config_set_values(device_config_t new_config) {
  Serial.println("CONFIG SET!");
  // led_set_color(LED_RED);
  // led_update();
  new_config.change_count++;
  config_store.write(new_config);
  // led_off();
}

device_config_t config_get_values() {
  return config_store.read();
};
