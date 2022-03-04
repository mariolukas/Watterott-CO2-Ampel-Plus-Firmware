#include "Sensor.h"
#include <JC_Button.h>
#include <SparkFun_SCD30_Arduino_Library.h>
#include <Wire.h>
#include "Buzzer.h"
#include "Config.h"
#include "DeviceConfig.h"
#include "LED.h"
#include "NetworkManager.h"
#if DISPLAY_OUTPUT > 0
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

#if DISPLAY_OUTPUT > 0
Adafruit_SSD1306 display(128, 64);  // 128x64 Pixel
//Serial.println("Use SSD1306 display");
#endif

SCD30 co2_sensor;
unsigned int co2 = STARTWERT, co2_average = STARTWERT;
unsigned int light = 1024;
float temp = 0, humi = 0;
static long long t_light = 0;
static int dunkel = 0;
uint8_t sensor_frc_allowed_timeout_ = 0;

void show_data(void)  // Daten anzeigen
{
#if SERIAL_OUTPUT > 0
  Serial.print("co2: ");
  Serial.println(co2);  // ppm
  Serial.print("temp: ");
  Serial.println(temp, 1);  //°C
  Serial.print("humidity: ");
  Serial.println(humi, 1);  //%
  Serial.print("light: ");
  Serial.println(light);
  if (wifi_is_connected()) {
    print_wifi_status();
  }
  Serial.println();
#endif

#if DISPLAY_OUTPUT > 0
  display.clearDisplay();
  display.setTextSize(5);
  display.setCursor(5, 5);
  display.println(co2);
  display.setTextSize(1);
  display.setCursor(5, 56);
  display.println("CO2 Level in ppm");
  display.display();
#endif

  return;
}

/// References:
/// - https://www.sensirion.com/en/download-center/carbon-dioxide-sensors-co2/co2-sensor/
/// - https://cdn.sparkfun.com/assets/4/8/8/7/7/Sensirion_CO2_Sensors_SCD30_Datasheet.pdf
/// - https://cdn.sparkfun.com/assets/f/e/d/0/1/Sensirion_CO2_Sensors_SCD30_Interface_Description.pdf
/// - https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library/blob/main/examples/Example4_EnableCalibrate/Example4_EnableCalibrate.ino

/*
  bool getAutoSelfCalibration(void);
  bool setAutoSelfCalibration(bool enable);
  bool getForcedRecalibration(uint16_t *val);
// Set the forced recalibration factor. See 1.3.7.
// The reference CO2 concentration has to be within the range 400 ppm ≤ cref(CO2) ≤ 2000 ppm.
  bool setForcedRecalibrationFactor(uint16_t concentration);
*/

/* Excerpt from SCD3 Interface Description:

1.4.5 (De-)Activate Automatic Self-Calibration (ASC)
Continuous automatic self-calibration can be (de-)activated with the following command. When activated for the first time a
period of minimum 7 days is needed so that the algorithm can find its initial parameter set for ASC. The sensor has to be exposed
to fresh air for at least 1 hour every day. Also during that period, the sensor may not be disconnected from the power supply,
otherwise the procedure to find calibration parameters is aborted and has to be restarted from the beginning. The successfully
calculated parameters are stored in non-volatile memory of the SCD30 having the effect that after a restart the previously found
parameters for ASC are still present. Note that the most recently found self-calibration parameters will be actively used for self-
calibration disregarding the status of this feature. Finding a new parameter set by the here described method will always
overwrite the settings from external recalibration (see chapter 0) and vice-versa. The feature is switched off by default.
To work properly SCD30 has to see fresh air on a regular basis. Optimal working conditions are given when the sensor sees
fresh air for one hour every day so that ASC can constantly re-calibrate. ASC only works in continuous measurement mode.
ASC status is saved in non-volatile memory. When the sensor is powered down while ASC is activated SCD30 will continue with
automatic self-calibration after repowering without sending the command.


Set Forced Recalibration value (FRC)
Forced recalibration (FRC) is used to compensate for sensor drifts when a reference value of the CO2 concentration in close
proximity to the SCD30 is available. For best results, the sensor has to be run in a stable environment in continuous mode at a
measurement rate of 2s for at least two minutes before applying the FRC command and sending the reference value. Setting a
reference CO2 concentration by the method described here will always supersede corrections from the ASC (see chapter 1.4.5)
and vice-versa. The reference CO2 concentration has to be within the range 400 ppm ≤ cref(CO2) ≤ 2000 ppm.
The FRC method imposes a permanent update of the CO2 calibration curve which persists after repowering the sensor. The
most recently used reference value is retained in volatile memory and can be read out with the command sequence given below.
After repowering the sensor, the command will return the standard reference value of 400 ppm.

*/

unsigned int light_sensor(void)  // Auslesen des Lichtsensors
{
  unsigned int i;
  uint32_t color = led_get_color();  // aktuelle Farbe speichern
  led_off();

  digitalWrite(PIN_LSENSOR_PWR, HIGH);  // Lichtsensor an
  delay(40);                            // 40ms warten
  i = analogRead(PIN_LSENSOR);          // 0...1024
  delay(10);                            // 10ms warten
  i += analogRead(PIN_LSENSOR);         // 0...1024
  i /= 2;
  digitalWrite(PIN_LSENSOR_PWR, LOW);  // Lichtsensor aus

  led_set_color(color);
  led_update();
  return i;
}

void sensor_init() {
  // co2_sensor.setForcedRecalibrationFactor(1135); //400ppm = Frischluft
  // //400ppm = Frischluft
  // co2_sensor.setMeasurementInterval(INTERVAL); //setze Messinterval
  // setze Pins
#if DEBUG_LOG > 0
  Serial.println("Initialize CO2 sensor");
#endif
  pinMode(PIN_LSENSOR_PWR, OUTPUT);
  digitalWrite(PIN_LSENSOR_PWR, LOW);  // Lichtsensor aus
  pinMode(PIN_LSENSOR, INPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);

  // Wire/I2C
  Wire.begin();
  Wire.setClock(50000);  // 50kHz, empfohlen fue SCD30

#if DISPLAY_OUTPUT > 0
  delay(500);  // 500ms warten
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
#endif

  while (co2_sensor.begin(Wire, AUTO_CALIBRATION) == false) {
    digitalWrite(PIN_LED, HIGH);
    delay(500);
    digitalWrite(PIN_LED, LOW);
    delay(500);
#if SERIAL_OUTPUT > 0
    Serial.println("Error: CO2 sensor not found.");
    led_failure(LED_RED);
#endif
  }
  // co2_sensor.setForcedRecalibrationFactor(1135);
  co2_sensor.setMeasurementInterval(INTERVAL);  // setze Messinterval
  delay(INTERVAL * 1000);                       // Intervallsekunden warten
  co2_sensor.setTemperatureOffset(TEMPERATURE_OFFSET);
}

void sensor_set_temperature_offset(float offset) {
  co2_sensor.setTemperatureOffset(offset);
#if DEBUG_LOG > 0
  Serial.print("Set temperatur offset to ");
  Serial.println(offset);
#endif
}

void sensor_handler(bool visualize_via_leds) {
  unsigned int ampel = 0;
  co2_average = (co2_average + co2) / 2;  // Berechnung jede Sekunde

#if USE_AVERAGE > 0
  ampel = co2_average;
#else
  ampel = co2;
#endif

  // neue Sensordaten auslesen
  if (co2_sensor.dataAvailable()) {
    co2 = co2_sensor.getCO2();
    temp = co2_sensor.getTemperature();
    humi = co2_sensor.getHumidity();
    if (wifi_is_connected()) {
      mqtt_send_value(co2, temp, humi, light);
    }

    show_data();
  }

  if (visualize_via_leds) {
    // Ampel
    if (ampel < START_YELLOW) {
      led_set_color(LED_GREEN);
    } else if (ampel < START_RED) {
      led_set_color(LED_YELLOW);
    } else if (ampel < START_RED_BLINK) {
      led_set_color(LED_RED);
    } else {  // rot blinken
      led_blink(LED_RED, 500);
    }

    led_update();  // zeige Farbe
  }

  //update timeout
  if (sensor_frc_allowed_timeout_ > 0) {
    sensor_frc_allowed_timeout_--;
  }

}

float get_temperature() {
  return temp;
}

unsigned int get_co2() {
  return co2;
}

float get_humidity() {
  return humi;
}

unsigned int get_brightness() {
  return light;
}

void sensor_handle_brightness() {
  if ((millis() - t_light) > (LIGHT_INTERVAL * 1000)) {
    t_light = millis();
    light = light_sensor();
    if (light < LIGHT_DARK && dunkel == 0) {
      dunkel = 1;
      co2_sensor.setMeasurementInterval(INTERVAL_DARK);
      led_adjust_brightness(255 / (100 / BRIGHTNESS_DARK));
    } else if (dunkel == 1) {
      dunkel = 0;
      co2_sensor.setMeasurementInterval(INTERVAL);
      led_adjust_brightness(255);  // 0...255
    }
  }
}

bool sensor_get_co2_autocalibration()
{
  return co2_sensor.getAutoSelfCalibration();
}

//switch on or off autocalibration for duration that we are powered (default is OFF)
void sensor_set_co2_autocalibration(bool enable)
{
  co2_sensor.setAutoSelfCalibration(enable);
#if SERIAL_OUTPUT > 0
  Serial.print("CO2 AutoCalibration: ");
  Serial.println(enable);
#endif
}

void sensor_allow_co2_force_recalibration(bool allow)
{
  if (allow)
  {
    sensor_frc_allowed_timeout_ = 60; //60*2s = 2min
#if SERIAL_OUTPUT > 0
    Serial.println("CO2 ForceReCalibration now allowed for 2min via MQTT");
#endif
  }
  else
    sensor_frc_allowed_timeout_ = 0;
}


// eg: use 400ppm = Frischluft
void sensor_do_co2_force_recalibration(uint32_t externally_accurately_measured_co2_value)
{
  if (externally_accurately_measured_co2_value < 400 || externally_accurately_measured_co2_value > 2000)
  {
    return;
  }
  if (0 == sensor_frc_allowed_timeout_)
  {
#if SERIAL_OUTPUT > 0
    Serial.println("CO2 Sensor ForceReCalibration current not allowed");
#endif
    return;
  }

  sensor_allow_co2_force_recalibration(false); //reset timeout

#if SERIAL_OUTPUT > 0
  Serial.println("CO2 Sensor ForceReCalibration started");
#endif

  //as a precaution, ensure measured value remains stable for 2minutes!!
  //also ensure, forced calibration is not more than 400ppm off from measured value

  unsigned int okay = 0, co2_last = co2, co2_first = co2;

  for (okay = 0; okay < 60;)
  {  // mindestens 60 Messungen (ca. 2 Minuten)

    led_one_by_one(LED_YELLOW, 100);
    led_update();

    if (co2_sensor.dataAvailable())  // alle 2s
    {
      co2 = co2_sensor.getCO2();
      temp = co2_sensor.getTemperature();
      humi = co2_sensor.getHumidity();

      if ((co2 > co2_first - 90) && (co2 < co2_first + 90) && (co2 > (co2_last - 30)) &&
          (co2 < (co2_last + 30)))  //+/-30ppm Toleranz zum vorherigen Wert
      {
        okay++;
      } else {
        okay = 0;
#if SERIAL_OUTPUT > 0
        Serial.println("FAIL: co2 measurement not stable");
#endif
        led_blink(LED_RED,400);
        led_blink(LED_RED,400);
        return; // ABORT
      }

      co2_last = co2;
      show_data();
    }
  }

  //done: co2 reference value has remained stable
  led_set_color(LED_GREEN);

#if SERIAL_OUTPUT > 0
  Serial.println("ok, values remained stable");
#endif

  co2_sensor.setForcedRecalibrationFactor(externally_accurately_measured_co2_value);
#if SERIAL_OUTPUT > 0
  Serial.println("CO2 Sensor ForceReCalibration finished");
#endif
  buzzer_ack();
  led_blink(LED_GREEN,900);
  led_blink(LED_GREEN,900);
}