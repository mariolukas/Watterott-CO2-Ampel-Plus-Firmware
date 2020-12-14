#ifndef MQTTManager_H
#define MQTTManager_H
#include "Led.h"
#include "NetworkManager.h"
#include <WiFi101.h>
#include <PubSubClient.h>

bool mqtt_connect();
void mqtt_send_value(int co2, int temp, int hum, int lux);
bool mqtt_broker_connected();

#endif
