#include "Config.h"
#include "Sensor.h"
#include "Buzzer.h"
#include "Led.h"
#include "NetworkManager.h"
#include "DeviceConfig.h"
#include <Wire.h>
#include <JC_Button.h> 
#include <SparkFun_SCD30_Arduino_Library.h>
#if DISPLAY_AUSGABE > 0
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

#if DISPLAY_AUSGABE > 0
Adafruit_SSD1306 display(128, 64); //128x64 Pixel
#endif

SCD30 co2_sensor;
unsigned int co2=STARTWERT, co2_average=STARTWERT;
unsigned int light=1024;
float temp=0, humi=0;
static long long t_light=0;
static int  dunkel=0;

void show_data(void) //Daten anzeigen
{
  #if SERIELLE_AUSGABE > 0
    Serial.print("co2: ");
    Serial.println(co2); //ppm
    Serial.print("temp: ");
    Serial.println(temp, 1); //°C
    Serial.print("humidity: ");
    Serial.println(humi, 1); //%
    Serial.print("light: ");
    Serial.println(light);
    if(wifi_is_connected()){
      print_wifi_status();
    }
    Serial.println();
  #endif

  #if DISPLAY_AUSGABE > 0
    display.clearDisplay();
    display.setTextSize(5);
    display.setCursor(5,5);
    display.println(co2);
    display.setTextSize(1);
    display.setCursor(5,56);
    display.println("CO2 Level in ppm");
    display.display();
  #endif

  return;
}

void sensor_calibration(){
  
  unsigned int okay=0, co2_last=0;
  //led_test();

  co2_last = co2;
  for(okay=0; okay < 60;){ //mindestens 60 Messungen (ca. 2 Minuten)

    led_one_by_one(LED_YELLOW, 100);
    led_update();

    if(co2_sensor.dataAvailable()) //alle 2s
    {
      co2  = co2_sensor.getCO2();
      temp = co2_sensor.getTemperature();
      humi = co2_sensor.getHumidity();

      if((co2 > 200) && (co2 < 600) && 
         (co2 > (co2_last-30)) &&
         (co2 < (co2_last+30))) //+/-30ppm Toleranz zum vorherigen Wert
      {
        okay++;
      }
      else
      {
        okay = 0;
      }
      
      co2_last = co2;

      if(co2 < 500)
      {
        led_set_color(LED_GREEN);
      }
      else if(co2 < 600)
      {
        led_set_color(LED_YELLOW);
      }
      else //>=600
      {
        led_set_color(LED_RED);
      }
      led_update();

      #if SERIELLE_AUSGABE > 0
        Serial.print("ok: ");
        Serial.println(okay);
      #endif

      show_data();
    }
    
    if(okay >= 60)
    {
      co2_sensor.setForcedRecalibrationFactor(400); //400ppm = Frischluft
      led_off();
      led_tick=0;
      delay(50);
      led_set_color(LED_GREEN);
      delay(100);
      led_off();
      led_update();
      buzzer_ack();
    }
  }
}

unsigned int light_sensor(void) //Auslesen des Lichtsensors
{
  unsigned int i;
  uint32_t color = led_get_color(); //aktuelle Farbe speichern
  led_off();

  digitalWrite(PIN_LSENSOR_PWR, HIGH); //Lichtsensor an
  delay(40); //40ms warten
  i = analogRead(PIN_LSENSOR); //0...1024
  delay(10); //10ms warten
  i += analogRead(PIN_LSENSOR); //0...1024
  i /= 2;
  digitalWrite(PIN_LSENSOR_PWR, LOW); //Lichtsensor aus

  led_set_color(color);
  led_update();  
  return i;
}

void sensor_init(){
  //co2_sensor.setForcedRecalibrationFactor(1135); //400ppm = Frischluft
  // //400ppm = Frischluft
  //co2_sensor.setMeasurementInterval(INTERVALL); //setze Messinterval 
  //setze Pins
  pinMode(PIN_LSENSOR_PWR, OUTPUT);
  digitalWrite(PIN_LSENSOR_PWR, LOW); //Lichtsensor aus
  pinMode(PIN_LSENSOR, INPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  
  //Wire/I2C
  Wire.begin();
  Wire.setClock(50000); //50kHz, empfohlen fue SCD30

  #if DISPLAY_AUSGABE > 0
    delay(500); //500ms warten
    display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  #endif

  while(co2_sensor.begin(Wire, AUTO_KALIBRIERUNG) == false)
  {
    digitalWrite(PIN_LED, HIGH);
    delay(500);
    digitalWrite(PIN_LED, LOW);
    delay(500);
    #if SERIELLE_AUSGABE > 0
      Serial.println("Fehler: CO2 Sensor nicht gefunden.");
      led_failure(LED_RED);
    #endif
  }
  //co2_sensor.setForcedRecalibrationFactor(1135);
  co2_sensor.setMeasurementInterval(INTERVALL); //setze Messinterval 
  delay(INTERVALL*1000); //Intervallsekunden warten
  co2_sensor.setTemperatureOffset(TEMPERATURE_OFFSET);  
  
}

void sensor_set_temperature_offset(float offset){
   co2_sensor.setTemperatureOffset(offset);  
}

void sensor_handler(){
  unsigned int ampel=0;
  co2_average = (co2_average + co2) / 2; //Berechnung jede Sekunde

  #if AMPEL_DURCHSCHNITT > 0
    ampel = co2_average;
  #else
    ampel = co2;
  #endif

 
  //neue Sensordaten auslesen
  if(co2_sensor.dataAvailable()){
    co2  = co2_sensor.getCO2();
    temp = co2_sensor.getTemperature();
    humi = co2_sensor.getHumidity();
    if (wifi_is_connected()){
      mqtt_send_value(MQTT_SENSOR_CO2, co2);
      mqtt_send_value(MQTT_SENSOR_TEMP, temp);
      mqtt_send_value(MQTT_SENSOR_HUM, humi);
    }
    
    show_data();
  }

    //Ampel
  if(ampel < START_GELB) {
    led_set_color(LED_GREEN);
  } else if(ampel < START_ROT) {
    led_set_color(LED_YELLOW);
  } else if(ampel < START_ROT_BLINKEN) {
    led_set_color(LED_RED);
  } else { //rot blinken
    led_blink(LED_RED, 500);
  }
  
  led_update(); //zeige Farbe
    
}

float get_temperature(){
  return temp;
}


unsigned int get_co2(){
  return co2;
}

float get_humidity(){
  return humi;
}

unsigned int get_brightness(){
  return light;  
}

void sensor_handle_brightness(){
    if((millis()-t_light) > (LICHT_INTERVALL*1000)){
      t_light = millis();
    light = light_sensor();
    mqtt_send_value(MQTT_SENSOR_LUX, light);
    if(light < LICHT_DUNKEL) {
        if(dunkel == 0) {
          dunkel = 1;
          co2_sensor.setMeasurementInterval(INTERVALL_DUNKEL); 
          led_adjust_brightness(255/(100/HELLIGKEIT_DUNKEL));
        }
    } else {
      if(dunkel == 1){
        dunkel = 0;
        co2_sensor.setMeasurementInterval(INTERVALL); 
        led_adjust_brightness(255); //0...255

      }
    }
  }
}
