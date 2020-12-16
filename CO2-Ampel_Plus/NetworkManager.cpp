#include "NetworkManager.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "RequestParser.h"
#include "Sensor.h"

char mdnsName[] = "wifi101";  // the MDNS name that the board will respond to
                              // after WiFi settings have been provisioned
// Note that the actual MDNS name will have '.local' after
// the name above, so "wifi101" will be accessible on
// the MDNS name "wifi101.local".

WiFiServer server(80);

// Create a MDNS responder to listen and respond to MDNS name requests.
// WiFiMDNSResponder mdnsResponder;

device_config_t cfg = config_get_values();
int wifi_status = WL_IDLE_STATUS;
byte wifi_mac[6];

bool wifi_is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

void wifi_ap_create() {
  led_set_color(LED_BLUE);
  led_update();

  if (wifi_status == WL_CONNECTED) {
    WiFi.end();
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true) {
      led_failure(LED_BLUE);
    }
  }
  WiFi.macAddress(wifi_mac);

  char ap_ssid[20];

  sprintf(ap_ssid, "%s %02X:%02X", AP_SSID, wifi_mac[4], wifi_mac[5]);
  wifi_status = WiFi.beginAP(ap_ssid, cfg.ap_password);
  if (wifi_status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true) {
      led_failure(LED_BLUE);
    }
  }
  delay(5000);

  print_wifi_status();
  server.begin();

  while (true) {
    wifi_handle_client();
  }
}

int wifi_wpa_connect() {
  if (wifi_status == WL_AP_CONNECTED) {
    WiFi.end();
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true) {
      led_failure(LED_BLUE);
    };
  }

  wifi_status = WiFi.begin(cfg.wifi_ssid, cfg.wifi_password);
  int timeout = 5000;

  while (timeout && (WiFi.status() != WL_CONNECTED)) {
    timeout -= 1000;
    led_failure(LED_BLUE);
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (strlen(cfg.wifi_ssid) == 0) {
      Serial.println("No SSID and Password set. Wifi connection failed");
    } else {
      Serial.print("Cennecting to ");
      Serial.print(cfg.wifi_ssid);
      Serial.println(" failed");
    }
  } else {
    print_wifi_status();
    server.begin();
    if (strcmp(cfg.mqtt_broker_address, "127.0.0.1")) {
      mqtt_connect();
    }
  }

  return wifi_status;
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
  Serial.print("signal strength (RSSI):");
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

void wifi_handle_client() {
  bool reboot = false;
  // compare the previous status to the current status
  if (wifi_status != WiFi.status()) {
    // it has changed update the variable
    wifi_status = WiFi.status();

    if (wifi_status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println(F("Device connected to AP"));
    } else {
      // a device has disconnected from the AP, and we are back in listening
      // mode
      Serial.println(F("Device disconnected from AP"));
    }
  }

  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {
    RequestParser requestParser = RequestParser(client);
    // if you get a client,
    Serial.println(
        F("New client Connected"));  // print a message out the serial port
    String currentLine =
        "";  // make a String to hold incoming data from the client

    while (client.connected()) {
      // loop while the client's connected
      if (client.available()) {
        // if there's bytes to read from the client,
        char c = client.read();  // read a byte, then
        requestParser.addHeaderCharacter(c);
        Serial.write(c);  // print it out the serial monitor

        if (c == '\n') {
          if (currentLine.length() == 0) {
            requestParser.grabPayload();
            if (requestParser.getPayload().length() > 0) {
              /**
               * Fill the config with the post values.
               */
              Serial.print("Broker ist ");
              Serial.println(requestParser.getField("broker"));

              if ((requestParser.getField("ssid").length() > 0)) {
                requestParser.getField("ssid").toCharArray(cfg.wifi_ssid, 40);
              }

              if ((requestParser.getField("pwd").length() > 0)) {
                requestParser.getField("pwd").toCharArray(cfg.wifi_password,
                                                          40);
              }

              if ((requestParser.getField("ap_pwd").length() > 0)) {
                requestParser.getField("ap_pwd").toCharArray(cfg.ap_password,
                                                             40);
              }

              if ((requestParser.getField("broker").length() > 0)) {
                requestParser.getField("broker").toCharArray(
                    cfg.mqtt_broker_address, 20);
              }

              if ((requestParser.getField("port").length() > 0)) {
                cfg.mqtt_broker_port = requestParser.getField("port").toInt();
              }

              if ((requestParser.getField("topic").length() > 0)) {
                requestParser.getField("topic").toCharArray(cfg.mqtt_topic, 20);
              }

              if ((requestParser.getField("mqttuser").length() > 0)) {
                requestParser.getField("mqttuser")
                    .toCharArray(cfg.mqtt_username, 20);
              }

              if ((requestParser.getField("mqttpass").length() > 0)) {
                requestParser.getField("mqttpass")
                    .toCharArray(cfg.mqtt_password, 20);
              }

              if ((requestParser.getField("ampel").length() > 0)) {
                requestParser.getField("ampel").toCharArray(cfg.ampel_name, 40);
              }

              if ((requestParser.getField("buzzer").length() > 0)) {
                if (requestParser.getField("buzzer") == "false") {
                  cfg.buzzer_enabled = false;
                } else {
                  cfg.buzzer_enabled = true;
                }
              }

              /**
               * Reboot if required.
               */
              if (reboot) {
                config_set_values(cfg);
                client.stop();
                NVIC_SystemReset();
              }
              cfg = config_get_values();
            }
            break;
          } else {
            // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {
          // if you got anything else but a carriage return character,
          currentLine += c;  // add it to the end of the currentLine
        }

        /**
         * WPA Connection Routes
         */
        if (wifi_status == WL_CONNECTED) {
          if (currentLine == F("GET /")) {
            cfg = config_get_values();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(wpa_root_html_header);
            client.print(cssampel);
            client.print(wpa_root_html_middle);
            client.print("<div class=\"box\"><h1>CO2 Ampel Status</h1>");
            client.print("<span class=\"css-ampel");
            int ampel = get_co2();
            if (ampel < START_GELB) {
              client.print(" ampelgruen");
            } else if (ampel < START_ROT) {
              client.print(" ampelgelb");
            } else if (ampel < START_ROT_BLINKEN) {
              client.print(" ampelrot");
            } else {  // rot blinken
              client.print(" ampelrotblinkend");
            }
            client.print("\"><span class=\"cssampelspan\"></span></span>");
            client.print("<br><br>");
            client.print("Co2: ");
            client.print(get_co2());
            client.print(" ppm<br>Temperatur: ");
            client.print(get_temperature());
            client.print(" &ordm;C<br>Luftfeuchtigkeit: ");
            client.print(get_humidity());
            client.print(" %<br>Helligkeit: ");
            int brgt = get_brightness();
            if (brgt == 1024) {
              client.print("--");
            } else {
              client.print(brgt);
            }

            client.print("<br>");
            client.print("<br>");
            client.print("MQTT Broker ist ");
            if (!mqtt_broker_connected()) {
              client.print("nicht ");
            }
            client.print("verbunden.");
            client.print("<br>");
            client.print("<br>");
            client.print("Firmware: ");
            client.println(VERSION);
            client.print("<br>");
            client.print("</div>");
            client.print(wpa_root_html_footer);
            client.println();
          }

          // Check to see if the client request was "GET /H" or "GET /L":
          if (currentLine.endsWith(F("POST /save"))) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(ap_save_html);
            client.println();
            config_set_values(cfg);

            reboot = true;
          }
        }

        /**
         * Access Point Routes
         */
        if (wifi_status == WL_AP_CONNECTED) {
          if (currentLine.endsWith(F("GET /"))) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(ap_root_html_header);
            client.print(
                "<form class=\"box\" action=\"/save\" "
                "method=\"POST\" "
                "name=\"loginForm\"><h1>Einstellungen</h1>");
            client.print("<label for=broker>MQTT Broker IP</label>");
            client.print(
                "<input name=broker "
                "placeholder='127.0.0.1' value='");
            client.print(cfg.mqtt_broker_address);
            client.print("'>");

            client.print("<label for=port>MQTT Broker Port</label>");
            client.print("<input name=port placeholder='1883' value='");
            client.print(cfg.mqtt_broker_port);
            client.print("'>");

            client.print("<label for=port>MQTT Base Topic</label>");
            client.print("<input name=topic placeholder='sensors' value='");
            client.print(cfg.mqtt_topic);
            client.print("'>");

            client.print("<label for=port>MQTT Username</label>");
            client.print(
                "<input name=mqttuser "
                "placeholder='username' value='");
            client.print(cfg.mqtt_username);
            client.print("'>");

            client.print("<label for=port>MQTT Password</label>");
            client.print(
                "<input type=password name=mqttpass "
                "placeholder='password' value='");
            client.print(cfg.mqtt_password);
            client.print("'>");

            client.print("<label for=ampel>Ampel Name</label>");
            client.print("<input name=ampel placeholder='Ampel_1' value='");
            client.print(cfg.ampel_name);
            client.print("'>");

            client.print("<label for=broker>SSID</label>");
            client.print("<input name=ssid placeholder='SSID' value='");
            client.print(cfg.wifi_ssid);
            client.print("'>");

            client.print("<label for=pwd>Password</label>");
            client.print(
                "<input type=password name=pwd "
                "placeholder='Passwort' value='");
            client.print(cfg.wifi_password);
            client.print("'>");

            client.print("<label for=ap_pwd>Access Point Passwort</label>");
            client.print(
                "<input type=password name=ap_pwd "
                "placeholder='Passwort' value='");
            client.print(cfg.ap_password);
            client.print("'>");

            client.print("<label for=buzzer>Buzzer</label>");
            client.print("<select id=buzzer name=buzzer size=2>");
            if (cfg.buzzer_enabled) {
              client.print(
                  "<option value=\"true\" "
                  "selected>Enabled</option>");
              client.print("<option value=\"false\">Disabled</option>");
            } else {
              client.print("<option value=\"true\">Enabled</option>");
              client.print(
                  "<option value=\"false\" "
                  "selected>Disabled</option>");
            };
            client.print("</select>");

            // client.print("<div class=\"btnbox\"><button
            // onclick=\"window.location.href='/selftest'\"
            // class=\"btn\">Selftest</button></div>"); client.print("<div
            // class=\"btnbox\"><button
            // onclick=\"window.location.href='/calibrate'\"
            // class=\"btn\">Calibration</button></div>");
            client.print(
                "<input type=submit class=btn "
                "value=\"Speichern und Neustart\">");
            client.print("<br><br>");
            client.print("Firmware: ");
            client.println(VERSION);
            client.print("</form>");

            client.print(ap_root_html_footer);
            client.println();
          }

          // Check to see if the client request was "GET /H" or "GET /L":
          if (currentLine.endsWith(F("POST /save"))) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(ap_save_html);
            client.println();
            reboot = true;
          }
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println(F("Client disconnected"));
  }
}
