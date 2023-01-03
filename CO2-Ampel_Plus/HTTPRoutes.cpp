#include "CO2Sensor.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "HTMLAPMode.h"
#include "HTMLWPAMode.h"
#include "LightSensor.h"
#include "NetworkManager.h"

void get_root(WiFiClient& client) {
  co2_sensor_measurement_t co2_sensor_measurement;
  bool measurement_valid = get_co2_sensor_measurement(co2_sensor_measurement);
  // TODO: do something with measurement_valid!

  device_config_t cfg = config_get_values();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print(wpa_root_html_header);
  client.print(cssampel);
  client.print(wpa_root_html_middle);
  client.print("<div class=\"box\"><h1>CO2 Ampel Status</h1>");
  client.print("<span class=\"css-ampel");
  int ampel = co2_sensor_measurement.co2;
  if (ampel < START_YELLOW) {
    client.print(" ampelgruen");
  } else if (ampel < START_RED) {
    client.print(" ampelgelb");
  } else if (ampel < START_RED_BLINK) {
    client.print(" ampelrot");
  } else {  // rot blinken
    client.print(" ampelrotblinkend");
  }
  client.print("\"><span class=\"cssampelspan\"></span></span>");
  client.print("<br><br>");
  client.print("Co2: ");
  client.print(co2_sensor_measurement.co2);
  client.print(" ppm<br>Temperatur: ");
  client.print(co2_sensor_measurement.temperature);
  client.print(" &ordm;C<br>Luftfeuchtigkeit: ");
  client.print(co2_sensor_measurement.humidity);
  client.print(" %<br>Helligkeit: ");
  int brgt = get_ambient_brightness();
  if (brgt == 1024) {
    client.print("--");
  } else {
    client.print(brgt);
  }

  client.print("<br><br>");
  client.print("MQTT Broker is ");
  if (!mqtt_broker_connected()) {
    client.print("not ");
  }
  client.print("connected.");
  client.print("<br><br>");
  client.print("Firmware: ");
  client.println(VERSION);
  client.print("<br>");
  client.print("</div>");
  client.print(wpa_root_html_footer);
  client.println();
}

void get_api_sensor(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println();

  DynamicJsonDocument doc(256);

  co2_sensor_measurement_t co2_sensor_measurement;
  bool measurement_valid = get_co2_sensor_measurement(co2_sensor_measurement);
  // TODO: do something with measurement_valid!

  doc["co2"] = co2_sensor_measurement.co2;
  doc["temperature"] = co2_sensor_measurement.temperature;
  doc["humidity"] = co2_sensor_measurement.humidity;
  doc["brightness"] = get_ambient_brightness();

  serializeJson(doc, client);
}
