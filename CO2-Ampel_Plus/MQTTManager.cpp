#include "MQTTManager.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "Led.h"

WiFiClient wifi_client;
PubSubClient mqttClient(wifi_client);

char* willMessage = "disconnected";
bool willRetain = true;
int willQoS = 1;

bool mqtt_connect() {
  device_config_t cfg = config_get_values();
  char willTopic[200];
  if (strcmp(cfg.mqtt_broker_address, "127.0.0.1")) { /* prevent connection to localhost */
    sprintf(willTopic, "%s/%s/%s", cfg.mqtt_topic, cfg.ampel_name,
            MQTT_LWT_SUBTOPIC);
    led_set_color(LED_WHITE);
    Serial.print("Connecting to ");
    Serial.print(cfg.mqtt_broker_address);
    Serial.print(":");
    Serial.println(cfg.mqtt_broker_port);
    mqttClient.setServer(cfg.mqtt_broker_address, cfg.mqtt_broker_port);
    if (!mqttClient.connect(cfg.ampel_name, cfg.mqtt_username, cfg.mqtt_password,
                            willTopic, willQoS, willRetain, willMessage)) {
      Serial.println("Could not connect to server.");
#if DEBUG_LOG > 0
      Serial.print("Confguration:");
      Serial.print("  Name: ");
      Serial.println(cfg.ampel_name);
      Serial.print("  Username: ");
      Serial.println(cfg.mqtt_username);
      Serial.print("  Password: ");
      Serial.println(cfg.mqtt_password);
#endif
      return false;
    }
    mqttClient.publish(willTopic, "connected");
    return true;
  }
}

bool mqtt_broker_connected() {
  return mqttClient.connected();
}

void mqtt_send_value(int co2, float temp, int hum, int lux) {
  if (mqttClient.connected()) {
    device_config_t cfg = config_get_values();
    char mqttTopic[128];
    char mqttMessage[512];
    char tempMessage[20];
    sprintf(tempMessage, "%d.%02d", (int)temp, (int)(temp*100)%100);

    if (cfg.mqtt_format == 0){    // sending data in JSON Format to specified topic...
      sprintf(mqttTopic, "%s/%s", cfg.mqtt_topic, cfg.ampel_name);

#if DEBUG_LOG > 0
      Serial.print("TempMQTTMessage: ");
      Serial.println(tempMessage);
#endif
      sprintf(mqttMessage,
              "{\"co2\":\"%i\",\"temp\":\" %s \",\"hum\":\"%i\",\"lux\":\"%i\"}",
              co2, tempMessage, hum, lux);
      if (mqttClient.publish(mqttTopic, mqttMessage)) {
        Serial.println("Data publication successfull.");
#if DEBUG_LOG > 0
        Serial.print("Message: ");
        Serial.println(mqttMessage);
        Serial.print("Topic: ");
        Serial.print(mqttTopic);
#endif
      } else {
        Serial.println(
            "Data publication failed, either connection lost or message too "
            "large.");
      };

    } else {    // sending data in influxdb format 
        
      sprintf(mqttMessage, "co2ampel,name=%s co2=%i,temp=%s,hum=%i,lux=%i", cfg.ampel_name, co2, tempMessage, hum, lux );
      if (mqttClient.publish(cfg.mqtt_topic, mqttMessage)) {
        Serial.println("Data publication successfull.");
        
#if DEBUG_LOG > 0
        Serial.print("Message: ");
        Serial.println(mqttMessage);
        Serial.print("Topic: ");
        Serial.print(mqttTopic);
#endif

      } else {
        Serial.println(
            "Data publication failed, either connection lost or message too "
            "large.");
      };
          
    }
    
    
  } else {
    Serial.println("Data publication failed, client is not connected. Trying to reconnect.");
    mqtt_connect();
  }
}
