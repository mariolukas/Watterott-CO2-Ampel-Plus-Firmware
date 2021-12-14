#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "v3.0.1"

#define DEBUG_LOG 0 // 1 = Enable debug log

//--- Measurement interval ---
#define INTERVAL 3          // 2-1800s (normal)
#define INTERVAL_DARK 30    // 2-1800s (at darkness)

//-- Button long press interval ---
#define LONG_PRESS_INTERVAL 3000 // ms

//--- CO2 limit values ---
// Covid Praevention:
// https://www.umwelt-campus.de/forschung/projekte/iot-werkstatt/ideen-zur-corona-krise
#define START_YELLOW 800        // >=  800ppm
#define START_RED 1000          // >= 1000ppm
#define START_RED_BLINK 1200    // >= 1200ppm
#define START_BUZZER 1400       // >= 1400ppm

// Ermuedung
/*
#define START_YELLOW 1000       // >= 1000ppm
#define START_RED 1200          // >= 1200ppm
#define START_RED_BLINK 1400    // >= 1400ppm
#define START_BUZZER 1600       // >= 1600ppm
*/

//--- WiFi configuration ---
#define WIFI_AP_SSID "CO2 Ampel"        // access point SSID
#define WIFI_AP_PASSWORD "co2admin"     // access point password
#define WIFI_WPA_SSID ""                // Network SSID
#define WIFI_WPA_PASSWORD ""            // Network password

//--- MQTT configuration ---
#define MQTT_DEVICE_NAME "Ampel_1"
#define MQTT_BROKER_PORT 1883
#define MQTT_BROKER_ADDR "127.0.0.1"
#define MQTT_TOPIC "sensors"
#define MQTT_LWT_SUBTOPIC "LWT"
#define MQTT_SET_SUBTOPIC "set"
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"
#define MQTT_SENSOR_CO2 888
#define MQTT_SENSOR_TEMP 889
#define MQTT_SENSOR_HUM 890
#define MQTT_SENSOR_LUX 891

/* MQTT Data Format.
 * 0 - JSON 
 * 1 - InfluxDB Line Format
 */
#define MQTT_FORMAT 0 

//--- Buzzer configuration ---
#define BUZZER_ENABLED false
#define BUZZER_VOLUME 255 / 128 // 1-255

//--- LED configuration ---
#define LIGHT_ENABLED true
#define BRIGHTNESS 180          // 1-255
#define BRIGHTNESS_DARK 5       // 1-100%
#define NUMBER_OF_WS2312_PIXELS 4
#define NUMBER_OF_WS2812_LEDS 4

#define LED_GREEN 0x00FF00
#define LED_YELLOW 0xFFFF00
#define LED_RED 0xFF0000
#define LED_BLUE 0X0000FF
#define LED_WHITE 0xFFFFFF
#define LED_VIOLET 0X9932CC

#define LED_COLOR_WIFI_MANAGER 0X0000FF
#define LED_COLOR_WIFI_CONNECTING 0X00FF00
#define LED_COLOR_WIFI_FAILURE 0X0000FF

//--- Light sensor configuration ---
#define LIGHT_DARK 20           // <20 -> dark
#define LIGHT_INTERVAL 1800     // 1-60000s (sensor test)

//--- Temperature sensor configuration
#define TEMPERATURE_OFFSET 13

//--- General settings ---
#define USE_AVERAGE 1           // 1 = Use the average CO2 value
#define AUTO_CALIBRATION 0
    // 1 = use auto-clibration (sensor must be on for 7 consecutive
    // days and exposed to fresh air at least one hour per day)
#define SERIAL_OUTPUT 1         // 1 = enable serial output (115200 bps)
#define DISPLAY_OUTPUT 0        // 1 = enable display output


#define STARTWERT 500  // 500ppm, CO2 starting value

//--- Only touch if you know what you're doing! ---
// STATES
#define WIFI_MODE_AP_INIT 90
#define WIFI_MODE_AP_LISTEN 91
#define WIFI_MODE_WPA_CONNECT 92
#define WIFI_MODE_WPA_LISTEN 88
#define WIFI_MODE_NOT_CONECTED 87
#define WIFI_RECONNECT_ATTEMPTS 10


// WIFI_LOGLEVEL below 4 causes EMPTY HTTP RESPONSE
// see: https://github.com/khoih-prog/WiFiWebServer/issues/3

#define DEBUG_WIFI_WEBSERVER_PORT Serial
#define _WIFI_LOGLEVEL_ 4
#define _WIFININA_LOGLEVEL_ 0

#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#define USE_WIFI_CUSTOM       false

#define BOARD_TYPE      "CO2 Ampel"
#define BOARD_NAME      
#define SHIELD_TYPE     "WiFi101 using WiFi101 Library"

#define HTTP_MAX_DATA_WAIT 5000  // ms to wait for the client to send the request
#define HTTP_MAX_POST_WAIT 5000  // ms to wait for POST data to arrive
#define HTTP_MAX_SEND_WAIT 5000  // ms to wait for data chunk to be ACKed
#define HTTP_MAX_CLOSE_WAIT 2000 // ms to wait for the client to close the connection

/*
#define PIN_BUZZER      (4u)
#define PIN_LSENSOR     (0u)
#define PIN_LSENSOR_PWR (1u)
*/

#endif
