#include "MQTTManager.h"
#include <ArduinoJson.h>
#include "Config.h"
#include "DeviceConfig.h"
#include "LED.h"
#include "LightSensor.h"
#include "scheduler.h"

WiFiClient wifi_client;
PubSubClient mqttClient(wifi_client);

const char* willMessage = "disconnected";
bool willRetain = true;
int willQoS = 1;

bool mqtt_connect() {
  device_config_t cfg = config_get_values();
  char willTopic[200];
  char setTopic[200];
  // prevent connection to localhost
  if (strcmp(cfg.mqtt_broker_address, "127.0.0.1")) {
    sprintf(willTopic, "%s/%s/%s", cfg.mqtt_topic, cfg.ampel_name,
            MQTT_LWT_SUBTOPIC);
    // led_set_color(LED_WHITE);
    Serial.print("Connecting to ");
    Serial.print(cfg.mqtt_broker_address);
    Serial.print(":");
    Serial.println(cfg.mqtt_broker_port);
    mqttClient.setServer(cfg.mqtt_broker_address, cfg.mqtt_broker_port);
    mqttClient.setCallback(mqtt_message_received);
    if (!mqttClient.connect(cfg.ampel_name, cfg.mqtt_username,
                            cfg.mqtt_password, willTopic, willQoS, willRetain,
                            willMessage)) {
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
    sprintf(setTopic, "%s/%s/%s", cfg.mqtt_topic, cfg.ampel_name,
            MQTT_SET_SUBTOPIC);
    if (mqttClient.subscribe(setTopic)) {
      Serial.print("Subscribed to topic ");
      Serial.println(setTopic);
    }
    return true;
  }
  return false;
}

enum MQTT_STATES {
  CONNECT,
  CONNECTED,
};

void mqtt();
Task task_mqtt(  //
    MQTT_LOOP_TASK_PERIOD_MS* TASK_MILLISECOND,
    -1,
    mqtt,
    &ts);

void mqtt() {
  static MQTT_STATES state = MQTT_STATES::CONNECT;

  switch (state) {
    case MQTT_STATES::CONNECT: {
      bool connected = mqtt_connect();
      if (connected == true) {
        state = MQTT_STATES::CONNECTED;
      } else {
        task_mqtt.delay(200);
      }
    } break;

    case MQTT_STATES::CONNECTED:
      mqttClient.loop();
      break;
  }
}

bool mqtt_broker_connected() {
  return mqttClient.connected();
}

void mqtt_send_value();
Task task_mqtt_send_value(  //
    10 * TASK_MILLISECOND,
    -1,
    mqtt_send_value,
    &ts);

void mqtt_send_value() {
  if (mqttClient.connected()) {
    device_config_t cfg = config_get_values();
    char mqttTopic[128];
    char mqttMessage[512];
    char tempMessage[20];

    co2_sensor_measurement_t co2_sensor_measurement;
    bool measurement_valid = get_co2_sensor_measurement(co2_sensor_measurement);

    int brightness = get_ambient_brightness();

    sprintf(tempMessage, "%d.%02d", co2_sensor_measurement.temperature,
            (int)(co2_sensor_measurement.temperature * 100) % 100);

    if (cfg.mqtt_format ==
        0) {  // sending data in JSON Format to specified topic...
      sprintf(mqttTopic, "%s/%s", cfg.mqtt_topic, cfg.ampel_name);

#if DEBUG_LOG > 0
      Serial.print("TempMQTTMessage: ");
      Serial.println(tempMessage);
#endif
      if (measurement_valid == true) {
        DynamicJsonDocument doc(256);
        doc["co2"] = co2_sensor_measurement.co2;
        doc["temp"] = co2_sensor_measurement.temperature;
        doc["hum"] = co2_sensor_measurement.humidity;
        doc["lux"] = get_ambient_brightness();
        serializeJson(doc, mqttMessage);
        if (mqttClient.publish(mqttTopic, mqttMessage)) {
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
    } else {  // sending data in influxdb format

      // TODO: this is probably broken!
      sprintf(mqttMessage, "co2ampel,name=%s,co2=%i,temp=%s,hum=%i,lux=%i",
              cfg.ampel_name, co2_sensor_measurement.co2, tempMessage,
              co2_sensor_measurement.humidity, brightness);
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
    Serial.println(
        "Data publication failed, client is not connected. Trying to "
        "reconnect.");
    mqtt_connect();
  }
  task_mqtt_send_value.disable();
}

void mqtt_message_received(char* topic, byte* payload, unsigned int length) {
  device_config_t cfg = config_get_values();
  String topicToProcess = topic;
  payload[length] = '\0';
  String payloadToProcess = (char*)payload;
#if DEBUG_LOG > 0
  Serial.print("Message arrived [");
  Serial.print(topicToProcess);
  Serial.print("]: ");
  Serial.println(topicToProcess);
#endif
  // Parse message into JSON
  DynamicJsonDocument doc(1024);
  auto error = deserializeJson(doc, payload);
  // Check if payloadToProcess is valid JSON
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }
  // Retrieve the values
  if (doc.containsKey("light_enabled")) {
    String light_enabled = doc["light_enabled"];
    if (light_enabled.equalsIgnoreCase(F("true"))) {
      Serial.println("Light enabled via MQTT");
      cfg.light_enabled = true;
    } else if (light_enabled.equalsIgnoreCase(F("false"))) {
      Serial.println("Light disabled via MQTT");
      cfg.light_enabled = false;
    }
    config_set_values(cfg);
    // led_set_brightness();
  }
  if (doc.containsKey("buzzer_enabled")) {
    String buzzer_enabled = doc["buzzer_enabled"];
    if (buzzer_enabled.equalsIgnoreCase(F("true"))) {
      Serial.println("Buzzer enabled via MQTT");
      cfg.buzzer_enabled = true;
    } else if (buzzer_enabled.equalsIgnoreCase(F("false"))) {
      Serial.println("Buzzer disabled via MQTT");
      cfg.buzzer_enabled = false;
    }
    config_set_values(cfg);
  }
}
