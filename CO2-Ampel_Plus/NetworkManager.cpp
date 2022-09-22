#include "NetworkManager.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "Sensor.h"
#include "HTMLStyles.h"
#include "HTMLJavaScript.h"
#include "HTMLStatic.h"
#include <WiFiWebServer.h>

device_config_t cfg = config_get_values();
int wifi_status = WL_IDLE_STATUS;
byte wifi_mac[6];
bool ap_mode_activated = false;

char mdnsName[] = "wifi101";  // the MDNS name that the board will respond to
                              // after WiFi settings have been provisioned
// Note that the actual MDNS name will have '.local' after
// the name above, so "wifi101" will be accessible on
// the MDNS name "wifi101.local".

WiFiWebServer server(80);


bool isAuthenticated(){
 
  if (server.hasHeader(F("Cookie"))){
    String cookie = server.header(F("Cookie"));
    if (cookie.indexOf(F("CO2SESSIONID=1")) != -1){
#if DEBUG_LOG > 0        
      Serial.println(F("Authentication Successful"));
#endif
      return true;
    }
  }
  return false;
}

void handleRoot(){  
  server.sendContent(root_header_html);
  
  server.sendContent("<style>");
  server.sendContent(css_styles_general);
  server.sendContent(css_styles_ampel);
  server.sendContent("</style>");
  
  server.sendContent(root_footer_html);
  server.sendContent("<script>");
  server.sendContent(javascript);
  server.sendContent("</script>");

}

void handleNotFound() {
  
  String message = F("File Not Found\n\n");
  
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, F("text/plain"), message);
  
}

/**
 * Save Settings Handler
 */
void handleSaveSettings(){
  if (server.method() != HTTP_POST){
    server.send(405, F("text/plain"), F("Method Not Allowed"));
  } else {

   if (server.hasArg(F("mqtt_broker_address"))){
      server.arg(F("mqtt_broker_address")).toCharArray(cfg.mqtt_broker_address, sizeof(cfg.mqtt_broker_address));;
   }

   if (server.hasArg(F("mqtt_broker_port"))){
      cfg.mqtt_broker_port = server.arg(F("mqtt_broker_port")).toInt();
   }

   if (server.hasArg(F("mqtt_topic"))){
      server.arg(F("mqtt_topic")).toCharArray(cfg.mqtt_topic, sizeof(cfg.mqtt_topic));
   }

   if (server.hasArg(F("mqtt_username"))){
      server.arg(F("mqtt_username")).toCharArray(cfg.mqtt_username, sizeof(cfg.mqtt_username));
   }

   if (server.hasArg(F("mqtt_password"))){
      server.arg(F("mqtt_password")).toCharArray(cfg.mqtt_password, sizeof(cfg.mqtt_password));
   }

   if (server.hasArg(F("ampel_name"))){
      server.arg(F("ampel_name")).toCharArray(cfg.ampel_name, sizeof(cfg.ampel_name));
   }

   if (server.hasArg(F("wifi_ssid"))){
      server.arg(F("wifi_ssid")).toCharArray(cfg.wifi_ssid, sizeof(cfg.wifi_ssid));
   }

   if (server.hasArg(F("wifi_password"))){
      server.arg(F("wifi_password")).toCharArray(cfg.wifi_password,sizeof(cfg.wifi_password));
   }

   if (server.hasArg(F("ap_password"))){
      server.arg(F("ap_password")).toCharArray(cfg.ap_password,sizeof(cfg.ap_password));
   }

  if (server.hasArg("buzzer_enabled")) {
     if (server.arg("buzzer_enabled") == "false") {
        cfg.buzzer_enabled = false;
     } else {
        cfg.buzzer_enabled = true;
     }
  }

  if (server.hasArg("light_enabled")) {
    if (server.arg("light_enabled") == "false") {
      cfg.light_enabled = false;
    } else {
      cfg.light_enabled = true;
    }
  }
              
  if (server.hasArg("mqtt_format")) {
      cfg.mqtt_format =  server.arg(F("mqtt_format")).toInt();
  }

   config_set_values(cfg);

  
   server.sendHeader(F("Location"), F("/"));
   /*
   server.sendHeader(F("Cache-Control"), F("no-cache"));
   server.sendHeader(F("Set-Cookie"), F("CO2SESSIONID=0"));
   */
   server.send(301);
    
  }
              
  server.stop();
  NVIC_SystemReset();
}

/**
 * Request Handler for Settings Form
 */
void handleSettings(){

  /*
  if (!isAuthenticated()){
    server.sendHeader(F("Location"), F("/login"));
    server.sendHeader(F("Cache-Control"), F("no-cache"));
    server.send(301);
    
    return;
  }
  */

  server.sendContent(settings_header_html);
  server.sendContent(css_styles_general);
  server.sendContent(settings_middle_html);
  
  server.sendContent(F("<label for=mqtt_broker_address>MQTT Broker IP</label>"));
  server.sendContent(F("<input name=mqtt_broker_address placeholder='127.0.0.1' value='"));
  server.sendContent(cfg.mqtt_broker_address);
  server.sendContent(F("'>"));

  server.sendContent(F("<label for=mqtt_broker_port>MQTT Broker Port</label>"));
  server.sendContent(F("<input name=mqtt_broker_port placeholder='1883' value='"));
  server.sendContent((const String) cfg.mqtt_broker_port);
  server.sendContent("'>");

  server.sendContent(F("<label for=mqtt_topic>MQTT Base Topic</label>"));
  server.sendContent(F("<input name=mqtt_topic placeholder='sensors' value='"));
  server.sendContent(cfg.mqtt_topic);
  server.sendContent(F("'>"));

  server.sendContent(F("<label for=mqtt_username>MQTT Username</label>"));
  server.sendContent(F("<input name=mqtt_username placeholder='username' value='"));
  server.sendContent(cfg.mqtt_username);
  server.sendContent(F("'>"));

  server.sendContent(F("<label for=mqtt_password>MQTT Password</label>"));
  server.sendContent(F("<input type=password name=mqtt_password placeholder='password' value='"));
  server.sendContent(cfg.mqtt_password);
  server.sendContent(F("'>"));

  server.sendContent(F("<label for=ampel_name>Ampel Name</label>"));
  server.sendContent(F("<input name=ampel_name placeholder='Ampel_1' value='"));
  server.sendContent(cfg.ampel_name);
  server.sendContent(F("'>"));

  server.sendContent(F("<label for=wifi_ssid>SSID</label>"));
  server.sendContent(F("<input name=wifi_ssid placeholder='SSID' value='"));
  server.sendContent(cfg.wifi_ssid);
  server.sendContent(F("'>"));

  server.sendContent(F("<label for=wifi_password>Password</label>"));
  server.sendContent(F("<input type=password name=wifi_password placeholder='Passwort' value='"));
  server.sendContent(cfg.wifi_password);
  server.sendContent(F("'>"));

  server.sendContent(F("<label for=ap_password>AP/Login Passwort</label>"));
  server.sendContent(F("<input type=password name=ap_password placeholder='Passwort' value='"));
  server.sendContent(cfg.ap_password);
  server.sendContent(F("'>"));

  server.sendContent(F("<label class=\"select-label\" for=buzzer_enabled>Buzzer</label>"));
  server.sendContent(F("<select class=\"select\" id=buzzer name=buzzer_enabled size=1>"));
  if (cfg.buzzer_enabled) {
    server.sendContent(F("<option value=\"true\" selected>Enabled</option>"));
    server.sendContent(F("<option value=\"false\">Disabled</option>"));
  } else {
    server.sendContent(F("<option value=\"true\">Enabled</option>"));
    server.sendContent(F("<option value=\"false\" selected>Disabled</option>"));
  };
  server.sendContent(F("</select><br><br>"));

  server.sendContent(F("<label class=\"select-label\" for=light_enabled>LEDs</label>"));
  server.sendContent(F("<select class=\"select\" id=led name=light_enabled size=1>"));
  if (cfg.light_enabled) {
    server.sendContent(F("<option value=\"true\" selected>Enabled</option>"));
    server.sendContent(F("<option value=\"false\">Disabled</option>"));
  } else {
    server.sendContent(F("<option value=\"true\">Enabled</option>"));
    server.sendContent(F("<option value=\"false\" selected>Disabled</option>"));
  };
  server.sendContent(F("</select><br><br>"));

  server.sendContent(F("<label class=\"select-label\" for=mqtt_format>MQTT Format</label>"));
  server.sendContent(F("<select class=\"select\" id=format name=mqtt_format size=1>"));
  if (cfg.mqtt_format == 0) {
    server.sendContent(F("<option value=\"0\" selected>JSON</option>"));
    server.sendContent(F("<option value=\"1\">Influx</option>"));
  } else {
    server.sendContent(F("<option value=\"0\">JSON</option>"));
    server.sendContent(F("<option value=\"1\" selected>Influx</option>"));
  };
  server.sendContent(F("</select>"));

  server.sendContent(F("<input type=submit class=btn value=\"Save and reboot\"><br><br>"));

  server.sendContent(F("Firmware: "));
  server.sendContent(VERSION);
  server.sendContent(F("</form>"));
  
  server.sendContent(settings_footer_html);

}

/*
void handleLogin(){
  String msg;
  
  if (server.hasHeader(F("Cookie"))){
#if DEBUG_LOG > 0
    Serial.print(F("Found cookie: "));
#endif
    String cookie = server.header(F("Cookie"));
#if DEBUG_LOG > 0
    Serial.println(cookie);
#endif
  }

  if (server.hasArg(F("login_password"))){
    if (server.arg(F("login_password")).equals(cfg.ap_password)){
      server.sendHeader(F("Location"), F("/settings"));
      server.sendHeader(F("Cache-Control"), F("no-cache"));
      server.sendHeader(F("Set-Cookie"), F("CO2SESSIONID=1"));
      server.send(301);
      Serial.println(F("Log in Successful"));
      return;
    }
    msg = F("Wrong password! try again.");
  
  }
  
  server.send(200, F("text/html"), login_html);
}

void handleLogout(){
   server.sendHeader(F("Location"), F("/"));
   server.sendHeader(F("Cache-Control"), F("no-cache"));
   server.sendHeader(F("Set-Cookie"), F("CO2SESSIONID=0"));
   server.send(301);
}
*/

void handleJSONResponse(){
  
  char jsonMessage[512];
  char tempMessage[10];
  char humMessage[10];
  char versionNumber[10];
  float temp = get_temperature();
  float hum = get_humidity();
  sprintf(tempMessage, "%d.%02d", (int)temp, (int)(temp*100)%100);
  sprintf(humMessage, "%d.%02d", (int)hum, (int)(hum*100)%100);
  sprintf(versionNumber, "%s", VERSION);

  
  sprintf(jsonMessage,
        "{\"co2\":\"%i\",\"temp\":\" %s \",\"hum\":\"%s\",\"lux\":\"%i\", \"mqtt\":\"%i\", \"firmware\":\"%s\", \"co2ASC\":%i}",
        get_co2(), tempMessage, humMessage, get_brightness(), mqtt_broker_connected(), versionNumber, sensor_get_co2_autocalibration());
  
  server.send(200,F("application/json"), jsonMessage);
}

void handleCSSAmpelFile(){
  server.send(200,F("text/css"), css_styles_ampel);
}

void handleCSSGeneralFile(){
  server.send(200,F("text/css"), css_styles_general);
}

void handleJavaScriptRequest(){
   server.send(200,F("text/javascript"), javascript);
}

void init_webserver_routes(int wifi_status){
  server.on(F("/styles.css"), handleCSSGeneralFile);

  if(wifi_status == WL_CONNECTED){
      /*
      server.on(F("/ampel.css"), handleCSSAmpelFile);
      server.on(F("/scripts.js"), handleJavaScriptRequest);
      server.on(F("/settings"), handleSettings);
      server.on(F("/login"), handleLogin);
      server.on(F("/logout"), handleLogout);
      */
      server.on(F("/sensors.json"), handleJSONResponse);
      server.on(F("/"), handleRoot);
  } else {
      server.on(F("/"), handleSettings);
      server.on(F("/save"), handleSaveSettings);
  }
  
}


bool wifi_is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

void wifi_ap_create() {

#if DEBUG_LOG > 0
  Serial.println("Create access point for configuration");
#endif
  
  ap_mode_activated = true;
  
  led_set_color(LED_COLOR_WIFI_MANAGER);
  led_set_brightness();
  led_update();
  
  if (wifi_status == WL_CONNECTED) {
    WiFi.end();
  }

  if (WiFi.status() == WL_NO_SHIELD) {
#if DEBUG_LOG > 0       
    Serial.println("WiFi shield not present");
#endif
    // don't continue
    while (true) {
      led_failure(LED_COLOR_WIFI_FAILURE);
    }
  }
  WiFi.macAddress(wifi_mac);

  char ap_ssid[20];
  
  sprintf(ap_ssid, "%s %02X:%02X", WIFI_AP_SSID, wifi_mac[4], wifi_mac[5]);
  wifi_status = WiFi.beginAP(ap_ssid, cfg.ap_password);
  if (wifi_status != WL_AP_LISTENING) {
#if DEBUG_LOG > 0       
    Serial.println("Creating access point failed");
#endif
    while (true) {
      led_failure(LED_COLOR_WIFI_FAILURE);
    }
  }
  delay(5000);
  
  init_webserver_routes(wifi_status);
  print_wifi_status();
  server.begin();

  while (true) {
    wifi_handle_client();
  }
}

bool ap_is_active(){
  return ap_mode_activated;
}

int wifi_wpa_connect() {
  if (wifi_status == WL_AP_CONNECTED) {
    WiFi.end();
    ap_mode_activated = false;
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
#if DEBUG_LOG > 0       
    Serial.println("WiFi shield not present");
#endif
    // don't continue
    while (true) {
      led_failure(LED_COLOR_WIFI_FAILURE);
    };
  }

  wifi_status = WiFi.begin(cfg.wifi_ssid, cfg.wifi_password);
  int timeout = 5000;

  while (timeout && (WiFi.status() != WL_CONNECTED)) {
    timeout -= 1000;
    led_failure(LED_COLOR_WIFI_CONNECTING);
  }

  if (WiFi.status() != WL_CONNECTED) {

#if DEBUG_LOG > 0       
    if (strlen(cfg.wifi_ssid) == 0) {
      Serial.println("No SSID and Password set. Wifi connection failed");
    } else {
      Serial.print("Cennecting to ");
      Serial.print(cfg.wifi_ssid);
      Serial.println(" failed");
    }
#endif

  } else {
    init_webserver_routes(wifi_status);
    print_wifi_status();
    server.begin();
    mqtt_connect();
  }

  return wifi_status;
}

void print_wifi_status() {
  // print the SSID of the network you're attached to:
#if DEBUG_LOG > 0       
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
#endif
}

void print_mac_address(byte mac[]) {
#if DEBUG_LOG > 0       
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
#endif
}


void wifi_handle_client() {


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
  
  server.handleClient();
  
 // WiFiClient client = server.available();  // listen for incoming clients

}
