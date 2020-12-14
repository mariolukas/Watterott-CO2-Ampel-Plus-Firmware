#include "Config.h"
#include "DeviceConfig.h"
#include "Led.h"
#include "MQTTManager.h"

WiFiClient wifi_client; 
PubSubClient mqttClient(wifi_client);

char *willMessage = "disconnected";
bool willRetain = true;
int willQoS = 1;

bool mqtt_connect(){
    device_config_t cfg = config_get_values();
    char willTopic[200];
    sprintf(willTopic, "%s/%s/%s", cfg.mqtt_topic, cfg.ampel_name, MQTT_LWT_SUBTOPIC);
    led_set_color(LED_WHITE);
    mqttClient.setServer(cfg.mqtt_broker_address, cfg.mqtt_broker_port); 
    if (!mqttClient.connect(cfg.ampel_name, NULL, NULL, willTopic, willQoS, willRetain, willMessage)) {
      return false;
    }
    mqttClient.publish(willTopic, "connected");
    return true;
}

bool mqtt_broker_connected(){
    return mqttClient.connected();
}

void mqtt_send_value(int co2, int temp, int hum, int lux){
    if(mqttClient.connected()){
        device_config_t cfg = config_get_values();
        char mqttTopic[128];
        sprintf(mqttTopic, "%s/%s", cfg.mqtt_topic, cfg.ampel_name);
        char mqttMessage[512];
        sprintf(mqttMessage, "{\"co2\":\"%i\",\"temp\":\"%i\",\"hum\":\"%i\",\"lux\":\"%i\"}", co2, temp, hum, lux);
        mqttClient.publish(mqttTopic, mqttMessage); 
    } 
}
