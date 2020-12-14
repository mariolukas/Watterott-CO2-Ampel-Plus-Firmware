#include "Config.h"
#include "DeviceConfig.h"
#include "Led.h"
#include "MQTTManager.h"

WiFiClient wifi_client; 
MqttClient mqttClient(wifi_client);

bool mqtt_connect(){
    device_config_t cfg = config_get_values();
    
    led_set_color(LED_WHITE);
    if (!mqttClient.connect(cfg.mqtt_broker_address, cfg.mqtt_broker_port)) {
      return false;
    }
    return true;
}

bool mqtt_broker_connected(){
    return mqttClient.connected();
}

void mqtt_send_value(int value_type, int value){

       if(mqttClient.connected()){
          
            device_config_t cfg = config_get_values();
            mqttClient.beginMessage(cfg.mqtt_topic);
            
            switch(value_type){
            case MQTT_SENSOR_CO2:
              mqttClient.print("co2,ampel=");
            break;
        
            case MQTT_SENSOR_TEMP:
              mqttClient.print("temp,ampel=");
    
            break;
        
            case MQTT_SENSOR_HUM:
              mqttClient.print("hum,ampel=");
            break;
    
            case MQTT_SENSOR_LUX:
              mqttClient.print("lux,ampel=");
            break;
            }
            
            mqttClient.print(cfg.ampel_name);
            mqttClient.print(" value=");
            
            mqttClient.print(value);
            mqttClient.endMessage(); 
      } 
}
