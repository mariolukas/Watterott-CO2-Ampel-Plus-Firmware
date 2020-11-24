#include "Config.h"
#include "Led.h"
#include "DeviceConfig.h"
FlashStorage(config_store, device_config_t);

bool config_is_initialized(){
    device_config_t _device_config;
    _device_config = config_store.read();
    if (_device_config.change_count > 0){
      return true;
    } else {
      return false;  
    }
}

void config_set_factory_defaults(){
  device_config_t _default_config = {1, "", "", WIFI_AP_PASSWORD, 1883, "127.0.0.1", "Ampel_1", 12.0};
  config_store.write(_default_config); 
}

void config_set_values(device_config_t new_config){
  led_set_color(LED_RED);
  led_update();
  new_config.change_count++;
  config_store.write(new_config);  
  delay(50); 
  led_off();
}

device_config_t config_get_values(){
    return config_store.read();
};
