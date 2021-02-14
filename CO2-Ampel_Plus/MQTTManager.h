#ifndef MQTTManager_H
#define MQTTManager_H
#include <PubSubClient.h>
#include <WiFi101.h>
#include "Led.h"
#include "NetworkManager.h"

bool mqtt_connect();
void mqtt_send_value(int co2, float temp, int hum, int lux);
bool mqtt_broker_connected();

#endif
