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
#include <vector>
#include "Buzzer.h"
#include "CO2Sensor.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "HTTP.h"
#include "LED.h"
#include "LEDPatterns.h"
#include "LightSensor.h"
#include "MQTTManager.h"
#include "NetworkManager.h"
#include "SerialCommand.h"
#include "scheduler.h"

#define HEARTBEAT_PERIOD 1000
void task_heartbeat_cb();
Task task(HEARTBEAT_PERIOD* TASK_MILLISECOND, -1, &task_heartbeat_cb);

void task_heartbeat_cb() {
  Serial.print("heartbeat ");
  Serial.println(task.getRunCounter());
}

void init_heartbeat(Scheduler& scheduler) {
  scheduler.addTask(task);
  task.enable();
}

byte wifi_state = WIFI_MODE_WPA_CONNECT;
const byte BUTTON_PIN(PIN_SWITCH);
const unsigned long LONG_PRESS(3000);

Button modeButton(BUTTON_PIN);

auto scheduler = TS::get_scheduler();

void setup() {
#if DEBUG_LOG > 0
  while (!Serial) {
    ;  // wait for serial port to connect.
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

  modeButton.begin();
  modeButton.read();

  // Factory Reset when button is pressed while reset
  if (!config_is_initialized() || modeButton.isPressed()) {
    Serial.println("Loading factory defaults");
    config_set_factory_defaults();
  }

  init_heartbeat(scheduler);
  buzzer_init();
  buzzer_test();

  init_leds(scheduler);
  led_state_queue.push(led_state_t{
      set_leds_circle_cw, 50, -1,
      std::vector<uint32_t>{LED_RED, LED_YELLOW, LED_GREEN, LED_BLUE}, 0});

  switch (wifi_state) {
      // // TODO: AP mode disabled for now
      // case WIFI_MODE_AP_INIT:  // Create  an Access  Point
      //   Serial.println("Creating Access Point");
      //   wifi_ap_create();
      //   wifi_state = WIFI_MODE_AP_LISTEN;
      //   Serial.println("------------------------");
      //   break;

    case WIFI_MODE_WPA_CONNECT:  // Connect to WiFi
      init_wifi_connect(scheduler);
      break;
  }

  init_serial(scheduler);

  bool all_tasks_done = false;
  do {
    bool task_wifi_connect_done = !task_wifi_connect.isEnabled();
    bool task_init_co2_sensor_done = !task_init_co2_sensor.isEnabled();
    all_tasks_done = task_wifi_connect_done && task_init_co2_sensor_done;

    scheduler.execute();
  } while (!all_tasks_done);

  // buzzer_queue_flush();
  led_default_on(LED_WHITE);
  led_queue_flush();

  Serial.println("Init complete!");
  Serial.println("------------------------");

  init_light_sensor(scheduler);
  init_co2_sensor(scheduler);
  init_http_server(scheduler);
  init_mqtt(scheduler);
}

void loop() {
  while (true) {
    scheduler.execute();
  }
  /**
   * Start WiFi Access Point when Button is pressed for more than 3 seconds
   */
  /*
  TODO: Handle this nicer
  modeButton.read();
  if (modeButton.pressedFor(3000)) {
    wifi_state = WIFI_MODE_AP_INIT;
  }
  */
}
