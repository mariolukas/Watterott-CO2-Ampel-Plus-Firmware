#ifndef MQTTManager_H
#define MQTTManager_H
#include "Led.h"
#include "NetworkManager.h"
#include <WiFi101.h>
#include <ArduinoMqttClient.h>

bool mqtt_connect();
void mqtt_send_value(int value_type, int value);
bool mqtt_broker_connected();

#endif
