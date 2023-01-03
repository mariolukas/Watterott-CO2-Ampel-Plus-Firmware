#ifndef LEDPATTERNS_H
#define LEDPATTERNS_H

void led_wifi_failure();
void led_wifi_connecting();
void led_wifi_connected();
void led_sensor_failure();
void led_off();
void led_on_color(uint32_t color);
void led_blink_color(uint32_t color);
void led_default_on(uint32_t color);
void led_default_blink(uint32_t color);

#endif
