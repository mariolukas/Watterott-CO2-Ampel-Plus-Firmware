#ifndef LED_H
#define LED_H
#include <Arduino.h>

extern byte led_tick;
// void led_pulse();
void fill_led_by_led(uint32_t color);
void led_ack();
void led_one_by_one(uint32_t color, int interval);
void led_off();
void led_test();
void led_set_color(uint32_t color);

void led_failure(uint32_t color);

void led_wifi_ap_mode();
void led_init();
void led_set_brightness();
void led_set_darkmode(bool darkmode);

void led_broker_connection_fail();

void led_blink(uint32_t color, int intervall);

void led_update();
void led_wpa_connecting_tick(bool is_on);

uint32_t led_get_color();

#endif
