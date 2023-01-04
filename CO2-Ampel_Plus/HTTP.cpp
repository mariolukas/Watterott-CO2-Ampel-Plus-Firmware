#include "HTTP.h"
#include <ArduinoJson.h>
#include <array>
#include "Config.h"
#include "HTTPRoutes.h"
#include "LightSensor.h"
#include "NetworkManager.h"

WiFiServer server(80);

template <size_t N>
void tokenize(std::array<String, N>& targets, String source, String delimiter) {
  int start = 0;
  int end = 0;
  for (auto& target : targets) {
    end = source.indexOf(delimiter, start);
    if (end != -1)
      target = source.substring(start, end);
    else {
      // If we find no match, the delimiter is either not present or it is the
      // part of the string after the last delimiter occurence
      target = source.substring(start, source.length());
      break;
    }
    start = end + 1;
  }
}

enum HTTP_SERVER_STATES {
  INIT,
  RUNNING,
};

void http_server();

Task task_http_server(  //
    HTTP_SERVER_TASK_PERIOD_MS* TASK_MILLISECOND,
    -1,
    &http_server);

void http_server() {
  bool reboot = false;
  bool respond = false;
  uint8_t wifi_status = get_wifi_status();
  static String request_string;

  static HTTP_SERVER_STATES state = HTTP_SERVER_STATES::INIT;

  switch (state) {
    case HTTP_SERVER_STATES::INIT:
      server.begin();
      request_string.reserve(512);
      state = HTTP_SERVER_STATES::RUNNING;
      break;
    case HTTP_SERVER_STATES::RUNNING:
      WiFiClient client = server.available();  // listen for incoming clients
      if (client) {
        // TODO: extend the parser to get request body for POST endpoints!
        std::array<String, 2> request_words;
        std::array<String, 2> url_parts;

        String& method = request_words[0];
        String& url = url_parts[0];
        String& params = url_parts[1];

        if (client.connected()) {
          while (client.available()) {
            request_string.concat(char(client.read()));
            Serial.println(request_string);

            if (request_string.endsWith("\r\n\r\n")) {
              tokenize(request_words, request_string, " ");
              tokenize(url_parts, request_words[1], "?");
              method = request_words[0];
              url = url_parts[0];
              params = url_parts[1];

              Serial.println(method);
              Serial.println(url);
              Serial.println(params);

              respond = true;
              break;
            }
          }

          if (respond == true) {
            // WPA Connection Routes
            if (wifi_status == WL_CONNECTED) {
              if (method == F("GET")) {
                if (url == F("/api/sensor")) {
                  get_api_sensor(client);
                } else if (url == F("/")) {
                  get_root(client);
                }
              }
              /*
              // TODO: not yet implemented
              if (method == F("POST")) {
                // TODO: Start of the config API
                if (url == F("/save")) {
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println();
                  client.print(ap_save_html);
                  client.println();
                  device_config_t cfg = config_get_values();
                  config_set_values(cfg);

                  reboot = true;
                }
              }
              */
            }

            /**
             * Access Point Routes
             */
            /*
            // TODO: AP mode disabled for now
            if (wifi_status == WL_AP_CONNECTED) {
              if (method == F("GET")) {
                if (url == F("/")) {
                  device_config_t cfg = config_get_values();
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println();
                  client.print(ap_root_html_header);
                  client.print(
                      "<form class=\"box\" action=\"/save\" "
                      "method=\"POST\" "
                      "name=\"loginForm\"><h1>Settings</h1>");
                  client.print("<label for=broker>MQTT Broker IP</label>");
                  client.print(
                      "<input name=broker placeholder='127.0.0.1' value='");
                  client.print(cfg.mqtt_broker_address);
                  client.print("'>");

                  client.print("<label for=port>MQTT Broker Port</label>");
                  client.print("<input name=port placeholder='1883' value='");
                  client.print(cfg.mqtt_broker_port);
                  client.print("'>");

                  client.print("<label for=port>MQTT Base Topic</label>");
                  client.print("<input name=topic placeholder='sensors'
            value='"); client.print(cfg.mqtt_topic); client.print("'>");

                  client.print("<label for=port>MQTT Username</label>");
                  client.print(
                      "<input name=mqttuser placeholder='username' value='");
                  client.print(cfg.mqtt_username);
                  client.print("'>");

                  client.print("<label for=port>MQTT Password</label>");
                  client.print(
                      "<input type=password name=mqttpass placeholder='password'
            " "value='"); client.print(cfg.mqtt_password); client.print("'>");

                  client.print("<label for=ampel>Ampel Name</label>");
                  client.print("<input name=ampel placeholder='Ampel_1'
            value='"); client.print(cfg.ampel_name); client.print("'>");

                  client.print("<label for=broker>SSID</label>");
                  client.print("<input name=ssid placeholder='SSID' value='");
                  client.print(cfg.wifi_ssid);
                  client.print("'>");

                  client.print("<label for=pwd>Password</label>");
                  client.print(
                      "<input type=password name=pwd placeholder='Passwort' "
                      "value='");
                  client.print(cfg.wifi_password);
                  client.print("'>");

                  client.print("<label for=ap_pwd>Access Point
            Passwort</label>"); client.print(
                      "<input type=password name=ap_pwd placeholder='Passwort' "
                      "value='");
                  client.print(cfg.ap_password);
                  client.print("'>");

                  client.print("<label for=buzzer>Buzzer</label>");
                  client.print("<select id=buzzer name=buzzer size=2>");
                  if (cfg.buzzer_enabled) {
                    client.print(
                        "<option value=\"true\" selected > Enabled</ option> ");
                    client.print(" < option value
            =\"false\">Disabled</option>"); } else { client.print("<option
            value=\"true\">Enabled</option>"); client.print(
                        "<option value=\"false\" selected>Disabled</option>");
                  };
                  client.print("</select>");
                  client.print("<br><br>");

                  client.print("<label for=led>LEDs</label>");
                  client.print("<select id=led name=led size=2>");
                  if (cfg.light_enabled) {
                    client.print(
                        "<option value=\"true\" selected > Enabled</ option> ");
                    client.print(" < option value
            =\"false\">Disabled</option>"); } else { client.print("<option
            value=\"true\">Enabled</option>"); client.print(
                        "<option value=\"false\" selected>Disabled</option>");
                  };
                  client.print("</select>");
                  client.print("<br><br>");

                  client.print("<label for=format>Format</label>");
                  client.print("<select id=format name=format size=2>");
                  if (cfg.mqtt_format == 0) {
                    client.print("<option value=\"0\" selected > JSON</ option>
            "); client.print(" < option value =\"1\">Influx</option>"); } else {
                    client.print("<option value=\"0\">JSON</option>");
                    client.print("<option value=\"1\"
            selected>Influx</option>");
                  };
                  client.print("</select>");

                  // client.print("<div class=\"btnbox\"><button
                  // onclick=\"window.location.href='/selftest'\"
                  // class=\"btn\">Selftest</button></div>");
                  client.print(
                      "<div class=\"btnbox\"><button "
                      "onclick=\"window.location.href='/calibrate'\" "
                      "class=\"btn\">Calibration</button></div>");
                  client.print(
                      "<input type=submit class=btn value=\"Save and
            reboot\">"); client.print("<br><br>"); client.print("Firmware: ");
                  client.println(VERSION);
                  client.print("</form>");

                  client.print(ap_root_html_footer);
                  client.println();
                }
              } else if (method == F("POST")) {
                // API should be the same for AP and WPA mode?
                if (url == F("/save")) {
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println();
                  client.print(ap_save_html);
                  client.println();
                  reboot = true;
                }
              }
            }
            */
            // close the connection:
            client.stop();
            request_string.remove(0);
          }
        }
      }
      break;
  }
}

/*
       TODO: Implement config as REST API
           requestParser.grabPayload();
           if (requestParser.getPayload().length() > 0) {
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

             if ((requestParser.getField("led").length() > 0)) {
               if (requestParser.getField("led") == "false") {
                 cfg.light_enabled = false;
               } else {
                 cfg.light_enabled = true;
               }
             }

             if ((requestParser.getField("format").length() > 0)) {
               cfg.mqtt_format = requestParser.getField("format").toInt();
             }
             if (reboot) {
               config_set_values(cfg);
               client.stop();
               NVIC_SystemReset();
             }
             cfg = config_get_values();
       */

void init_http_server(Scheduler& scheduler) {
  scheduler.addTask(task_http_server);
  task_http_server.enable();
}
