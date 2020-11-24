#include "Arduino.h"
#include "Config.h"
#include "Buzzer.h"

void buzzer_init(){
   pinMode(PIN_BUZZER, OUTPUT);
   digitalWrite(PIN_BUZZER, LOW);
}

void buzzer_test(){
  buzzer_on();
  delay(1000);
  buzzer_off();  
}

void buzzer_ack(){
  buzzer_on();
  delay(500);
  buzzer_off();
}

void buzzer_on(){
     analogWrite(PIN_BUZZER, 255/2); //Buzzer an  
}

void buzzer_off(){
     analogWrite(PIN_BUZZER, 0); //Buzzer aus
}
  
