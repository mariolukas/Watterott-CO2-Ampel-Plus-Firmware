#ifndef NETWORK_H
#define NETWORK_H
//#include "MQTTManager.h"
#include "Sensor.h"
#include "Buzzer.h"
#include "Led.h"
#include "HTMLAPMode.h"
#include "HTMLWPAMode.h"
#include "MQTTManager.h"
#include <SPI.h>
#include <WiFi101.h>

bool wifi_is_connected();

void wifi_ap_create();

void wifi_handle_ap_html();

int wifi_wpa_connect();

void wpa_listen_handler();

void print_wifi_status();

void print_wifi_status();

void print_mac_address(byte mac[]);
void wifi_handle_client();

#endif
