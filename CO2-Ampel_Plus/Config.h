#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "v2.0.0"

#define WS2312_NUMBER_OF_PIXELS 4
// STATES
#define WIFI_MODE_AP_INIT 90
#define WIFI_MODE_AP_LISTEN 91
#define WIFI_MODE_WPA_CONNECT 92
#define WIFI_MODE_WPA_LISTEN 88
#define WIFI_MODE_NOT_CONECTED 87
#define WIFI_RECONNECT_ATTEMPTS 10
#define WIFI_AP_PASSWORD "co2admin"


//  Required for WiFiWebServer lib
#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#define DEBUG_WIFI_WEBSERVER_PORT   Serial
#define _WIFI_LOGLEVEL_             3
#define _WIFININA_LOGLEVEL_         3

#define HTTP_MAX_DATA_WAIT    5000 //ms to wait for the client to send the request
#define HTTP_MAX_POST_WAIT    5000 //ms to wait for POST data to arrive
#define HTTP_MAX_SEND_WAIT    5000 //ms to wait for data chunk to be ACKed
#define HTTP_MAX_CLOSE_WAIT   2000 //ms to wait for the client to close the connection

/*
#define PIN_BUZZER      (4u)
#define PIN_LSENSOR     (0u)
#define PIN_LSENSOR_PWR (1u)
*/

//--- Messintervall ---
#define INTERVALL            3 //2-1800s (normal)
#define INTERVALL_DUNKEL    30 //2-1800s (bei Dunkelheit)

//-- Button long press interval in ms ---
#define LONG_PRESS_INTERVAL 3000

//--- CO2-Werte ---
//Covid Praevention: https://www.umwelt-campus.de/forschung/projekte/iot-werkstatt/ideen-zur-corona-krise
#define START_GELB          800 //>= 800ppm
#define START_ROT          1000 //>=1000ppm
#define START_ROT_BLINKEN  1200 //>=1200ppm
#define START_BUZZER       1400 //>=1400ppm

//Ermuedung
/*
#define START_GELB         1000 //>=1000ppm
#define START_ROT          1200 //>=1200ppm
#define START_ROT_BLINKEN  1400 //>=1400ppm
#define START_BUZZER       1600 //>=1600ppm
*/
#define AP_SSID "CO2 Ampel"

#define DEVICE_NAME "Ampel_1"

#define MQTT_BROKER_PORT 1883
#define MQTT_BROKER_ADDR "127.0.0.1"
#define MQTT_TOPIC "sensors"
#define MQTT_LWT_SUBTOPIC "LWT" 

#define MQTT_SENSOR_CO2 888
#define MQTT_SENSOR_TEMP 889
#define MQTT_SENSOR_HUM 890
#define MQTT_SENSOR_LUX 891

#define NUMBER_OF_WS2812_LEDS 4


//--- Ampelhelligkeit (LEDs) ---
#define HELLIGKEIT         180 //1-255 
#define HELLIGKEIT_DUNKEL  5  //1-100%

//--- Lichtsensor ---
#define LICHT_DUNKEL       20   //<20 -> dunkel
#define LICHT_INTERVALL    1800 //1-60000s (Sensorpruefung)

#define TEMPERATURE_OFFSET 13

//--- Allgemein ---
#define AMPEL_DURCHSCHNITT 1 //1 = CO2 Durchschnitt fuer Ampel verwenden
#define AUTO_KALIBRIERUNG  0 //1 = automatische Kalibrierung an (erfordert 7 Tage Dauerbetrieb mit 1h Frischluft pro Tag)
#define SERIELLE_AUSGABE   1 //1 = serielle Ausgabe aktivieren (9600 Baud)
#define DISPLAY_AUSGABE    0 //1 = Ausgabe auf Display aktivieren


#define LED_GREEN        0x00FF00
#define LED_YELLOW       0xFFFF00   
#define LED_RED          0xFF0000
#define LED_BLUE         0X0000FF
#define LED_WHITE        0xFFFFFF
#define LED_VIOLET       0X9932CC

#define COLOR_WIFI_MANAGER 0X0000FF
#define COLOR_WIFI_CONNECTING 0X00FF00

#define STARTWERT          500 //500ppm, CO2-Startwert
#endif
