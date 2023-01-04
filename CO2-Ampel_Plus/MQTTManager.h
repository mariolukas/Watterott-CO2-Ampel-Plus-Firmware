#ifndef MQTTManager_H
#define MQTTManager_H
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi101.h>
#include "LED.h"
#include "NetworkManager.h"

constexpr uint32_t MQTT_LOOP_TASK_PERIOD_MS = 10;

extern Task task_mqtt_send_value;

bool mqtt_connect();
bool mqtt_broker_connected();
void mqtt_message_received(char* topic, byte* payload, unsigned int length);
void init_mqtt(Scheduler& scheduler);

#endif
