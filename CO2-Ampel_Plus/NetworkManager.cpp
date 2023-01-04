#include "NetworkManager.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "LEDPatterns.h"

device_config_t cfg = config_get_values();
byte wifi_mac[6];
bool ap_mode_activated = false;

bool wifi_is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

uint8_t get_wifi_status() {
  return WiFi.status();
}

/*
// TODO: AP mode disabled for now
void wifi_ap_create() {
#if DEBUG_LOG > 0
  Serial.println("Create access point for configuration");
#endif

  ap_mode_activated = true;

  // led_set_color(LED_COLOR_WIFI_MANAGER);
  // led_set_brightness();
  // led_update();

  if (wifi_status == WL_CONNECTED) {
    WiFi.end();
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true) {
      // TODO: Think of a better way to handle final errors
      // led_failure(LED_COLOR_WIFI_FAILURE);
    }
  }
  WiFi.macAddress(wifi_mac);

  char ap_ssid[20];

  sprintf(ap_ssid, "%s %02X:%02X", WIFI_AP_SSID, wifi_mac[4], wifi_mac[5]);
  wifi_status = WiFi.beginAP(ap_ssid, cfg.ap_password);
  if (wifi_status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true) {
      // TODO: Think of a better way to handle final errors
      // led_failure(LED_COLOR_WIFI_FAILURE);
    }
  }
  delay(5000);

  // TODO: print_wifi_status();

  // TODO: just no.
  // server.begin();

  while (true) {
    // TODO: just no.
    // wifi_handle_client();
  }
}

bool ap_is_active() {
  return ap_mode_activated;
}
*/

enum WIFI_CONNECT_STATES { INIT, CONNECTING, CONNECTED, FAILURE, TIMEOUT };
WIFI_CONNECT_STATES wifi_connect_state = WIFI_CONNECT_STATES::INIT;

void wifi_wpa_connect();

Task task_wifi_connect(500 * TASK_MILLISECOND, -1, &wifi_wpa_connect);

void wifi_wpa_connect() {
  uint8_t wifi_status = WiFi.status();
  static int started_connecting_run_count;
  int connect_try_count = 0;
  switch (wifi_connect_state) {
    default:
    case WIFI_CONNECT_STATES::INIT:
      if (wifi_status == WL_AP_CONNECTED) {
        WiFi.end();
        ap_mode_activated = false;
      }
      // check for the presence of the shield:
      if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        wifi_connect_state = WIFI_CONNECT_STATES::FAILURE;
        led_wifi_failure();
      }

      cfg = config_get_values();
      if (strlen(cfg.wifi_ssid) == 0 || strlen(cfg.wifi_password) == 0) {
        Serial.println("Wifi SSID and/or password not set!");
        wifi_connect_state = WIFI_CONNECT_STATES::INIT;
        task_wifi_connect.delay(200);
        break;
      }

      WiFi.begin(cfg.wifi_ssid, cfg.wifi_password);
      started_connecting_run_count = task_wifi_connect.getRunCounter();
      wifi_connect_state = WIFI_CONNECT_STATES::CONNECTING;
      break;

    case WIFI_CONNECT_STATES::CONNECTING:
      connect_try_count =
          task_wifi_connect.getRunCounter() - started_connecting_run_count;
      if (wifi_status == WL_CONNECTED) {
        wifi_connect_state = WIFI_CONNECT_STATES::CONNECTED;
      } else if (connect_try_count >= 10) {
        wifi_connect_state = WIFI_CONNECT_STATES::TIMEOUT;
      }
      break;

    case WIFI_CONNECT_STATES::TIMEOUT:
      Serial.print("Timeout connecting to wifi!");
      Serial.println(cfg.wifi_ssid);
      wifi_connect_state = WIFI_CONNECT_STATES::INIT;
      task_wifi_connect.restartDelayed(1000);
      break;

    case WIFI_CONNECT_STATES::CONNECTED:
      print_wifi_status();
      task_wifi_connect.disable();
      break;

    case WIFI_CONNECT_STATES::FAILURE:
      // TODO: What do we do here? Reboot?
      break;
  }
}

void init_wifi_connect(Scheduler& scheduler) {
  scheduler.addTask(task_wifi_connect);
  task_wifi_connect.enable();
}

void print_wifi_status() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void print_mac_address(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
