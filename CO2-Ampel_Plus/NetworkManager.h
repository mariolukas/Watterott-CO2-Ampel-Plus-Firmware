#ifndef NETWORK_H
#define NETWORK_H
#include "Buzzer.h"
#include "LED.h"
#include "MQTTManager.h"
#include "Sensor.h"


bool wifi_is_connected();


void handleNotFound();
void handleRoot();
void handleCSSFile();

void wifi_ap_create();

bool ap_is_active();

void wifi_handle_ap_html();

int wifi_wpa_connect();

void wpa_listen_handler();

void print_wifi_status();

void print_wifi_status();

void print_mac_address(byte mac[]);
void wifi_handle_client();
void init_webserver_routes();


#endif
