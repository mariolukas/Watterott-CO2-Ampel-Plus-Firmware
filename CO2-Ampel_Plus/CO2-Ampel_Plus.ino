/*
  CO2-Ampel (v2.2.0)

  Testmodus:
  1. Den Switch-Taster beim Einschalten gedrueckt halten.
  2. Buzzer-Test
  3. LED-Test: rot, gruen, blau
  4. Sensor-Test: LED 1 = Licht, LED 2 = CO2, LED 3 = Temperatur, LED 4 =
  Luftfeuchtigkeit

  Kalibrierung:
  1. Die Ampel bei Frischluft mind. 1 Minute betreiben (im Freien oder am
  offenen Fenster, aber windgeschützt).
  2. Den Testmodus starten.
  3. Nach dem LED-Test (blaue LEDs) den Switch-Taster waehrend des Sensor-Tests
  kurz drücken (Buzzer ertoent).
  4. Die Kalibrierung wird nach dem Sensor-Test ausgeführt und dauert mindestens
  2 Minuten. Die LEDs zeigen dabei den aktuellen CO2-Wert an: gruen bis 499ppm,
  gelb bis 599ppm, rot ab 600ppm
  5. Nach erfolgreicher Kalibrierung leuchten die LEDs kurz blau und der Buzzer
  ertoent.
*/
#include <Arduino.h>
#include <JC_Button.h>
#include <WiFi101.h>
#include "Buzzer.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "LED.h"
#include "NetworkManager.h"
#include "Sensor.h"
#include "MQTTManager.h"

byte wifi_state = WIFI_MODE_WPA_CONNECT;
uint8_t button_press_ctr_=0;
const byte BUTTON_PIN(PIN_SWITCH);
const unsigned long LONG_PRESS(3000);

Button modeButton(BUTTON_PIN);

int wifi_reconnect_attemps = WIFI_RECONNECT_ATTEMPTS;

void setup() {


  
#if DEBUG_LOG > 0  
    while (!Serial) {
     ; // wait for serial port to connect.
  }
#endif
  Serial.begin(115200);
  Serial.println("------------------------");
  Serial.println("Starting setup...");
  Serial.println("Watterott CO2 Ampel PLUS");
  Serial.print("Firmware version ");
  Serial.println(VERSION);
#if DEBUG_LOG > 0
  Serial.println("Loglevel set to DEBUG!");
  Serial.println("--- !!! WARNING !!! ---");
  Serial.println("Debug log exposes sensitive");
  Serial.println("login informtation to the");
  Serial.println("serial console. Never use");
  Serial.println("in production environment!");
#endif

  led_init();
  led_test();
  led_set_color(LED_WHITE);
  led_update();


  modeButton.begin();
  modeButton.read();

  buzzer_init();
  buzzer_test();

  sensor_init();
  /**
   * Factory Reset when button is pressed while reset
   */
  if (!config_is_initialized() || modeButton.isPressed()) {
#if DEBUG_LOG > 0  
    Serial.println("Loading factory defaults");
#endif
    led_off();
    led_set_color(LED_RED);
    led_update();
    delay(50);
    config_set_factory_defaults();
    led_off();
  }
#if DEBUG_LOG > 0  
  Serial.println("Setup complete!");
  Serial.println("------------------------");
#endif
Serial.print("CO2 Sensor AutoCalibration is: ");
Serial.println(sensor_get_co2_autocalibration());
}

void handle_button_action(uint8_t action) {
  buzzer_ack();
  fill_num_leds(LED_DARKVIOLET, action);
  delay(700);
  switch(action) {
    case 1:
      if (config_is_initialized()) {
        device_config_t cfg = config_get_values();
        cfg.buzzer_enabled = !cfg.buzzer_enabled;
        config_set_values(cfg);
        if (cfg.buzzer_enabled)
        {
          buzzer_ack(); //if  on: beep twice
          delay(200);   //if off: beep once
        }
        buzzer_ack();
      }
      break;
    case 2:
      sensor_set_co2_autocalibration(false);
      led_blink(LED_DARKGREEN, 600);
      delay(400);
      break;
    case 3:
      sensor_set_co2_autocalibration(true);
      led_blink(LED_GREEN, 600);
      delay(400);
      break;
    case 4:
      sensor_allow_co2_force_recalibration(true);
      led_blink(LED_YELLOW, 600);
      delay(400);
      break;
    default:
      return;
  }
}

void loop() {
  /**
   * Start WiFi Access Point when Button is pressed for more than 3 seconds
   */
  modeButton.read();
  if (modeButton.pressedFor(3000)) {
    wifi_state = WIFI_MODE_AP_INIT;
    button_press_ctr_ = 0;
  } else if (button_press_ctr_ > 0 && modeButton.releasedFor(1900)) {
    handle_button_action(button_press_ctr_);
    button_press_ctr_ = 0;
  } else if (modeButton.wasPressed()) {
    button_press_ctr_++;
    button_press_ctr_ %= 5;
#if DEBUG_LOG > 0
    Serial.print("Button Press ");
    Serial.println(button_press_ctr_);
#endif
    delay(100);
  }

  switch (wifi_state) {
    case WIFI_MODE_AP_INIT:  // Create  an Access  Point
#if DEBUG_LOG > 0
      Serial.println("Creating Access Point");
#endif
      wifi_ap_create();
      wifi_state = WIFI_MODE_AP_LISTEN;
#if DEBUG_LOG > 0  
      Serial.println("------------------------");
#endif      
      break;

    case WIFI_MODE_WPA_CONNECT:  // Connect to WiFi

      device_config_t cfg = config_get_values();

#if DEBUG_LOG > 0  
      Serial.print("Connecting to SSID ");
      Serial.print(cfg.wifi_ssid);
      Serial.println(" Wifi");
#endif      
      if (strlen(cfg.wifi_ssid) != 0) {
        if (wifi_wpa_connect() == WL_CONNECTED) {
          wifi_state = WIFI_MODE_WPA_LISTEN;
        } else {
          wifi_state = WIFI_MODE_WPA_CONNECT;
        }
      } else {
        Serial.println("No WiFi SSID Configured.");
        wifi_state = WIFI_MODE_NOT_CONECTED;
      }
#if DEBUG_LOG > 0  
      Serial.println("------------------------");
      Serial.println("Start measurement");
      Serial.println("");
#endif
      break;
  }

  if (!wifi_is_connected()) {
    wifi_state = WIFI_MODE_WPA_CONNECT;
  }


  mqtt_loop();
  sensor_handler(0 == button_press_ctr_);
  if (button_press_ctr_ > 0)
  {
    fill_num_leds(LED_DARKYELLOW, button_press_ctr_);     //visualize button
  }
  sensor_handle_brightness();
  wifi_handle_client();

}
