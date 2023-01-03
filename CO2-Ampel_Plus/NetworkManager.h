#ifndef NETWORK_H
#define NETWORK_H
#include <SPI.h>
#include <WiFi101.h>
#include "CO2Sensor.h"
#include "MQTTManager.h"
#include "scheduler.h"

bool wifi_is_connected();
uint8_t get_wifi_status();

/*
// TODO: AP mode disabled for now
void wifi_ap_create();
bool ap_is_active();
void wifi_handle_ap_html();
*/

void print_wifi_status();

extern Task task_wifi_connect;

#endif
