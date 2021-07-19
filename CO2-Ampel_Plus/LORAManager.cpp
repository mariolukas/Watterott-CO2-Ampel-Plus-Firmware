#include "Config.h"
#include "DeviceConfig.h"
#include "LORAManager.h"
#include "Sensor.h"
#include <LoraMessage.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

static osjob_t sendjob;


//static const u1_t PROGMEM APPEUI[8]={ 'Your APP ID Here' };
static const u1_t APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
//static const u1_t PROGMEM DEVEUI[8]={ 'YOUR DEVICE ID HERE '};
static const u1_t DEVEUI[8] = {   0x47,0xE3,0x83, 0x13, 0x5E,0xE7,0xFF,0x26   };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
//static const u1_t PROGMEM APPKEY[16] = { 'YOUR APP KEY HERE '};
static const u1_t APPKEY[16] = { 0x89, 0x55, 0xAC, 0x2D, 0x08, 0xFC, 0x90, 0xEB, 0x21, 0xE5, 0x4E, 0xB0, 0x8C, 0x85, 0xAE, 0x6A };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}



// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;


const lmic_pinmap lmic_pins = {
    .nss = LORA_NSS_PIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LMIC_UNUSED_PIN,
    .dio = {LORA_DIO_PIN1, LORA_DIO_PIN2, LMIC_UNUSED_PIN},
};


void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        LoraMessage message;

        // add co2 value
        message.addUint16(get_co2());
        
       
        delay(2000);

        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, message.getBytes(), message.getLength(), 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}


void eui_to_string(byte eui[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (eui[i] >> 4) & 0x0F;
        byte nib2 = (eui[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

/*

byte array[4] = {0xAB, 0xCD, 0xEF, 0x99};
char str[32] = "";
array_to_string(array, 4, str);

 */

uint8_t string_to_eui(char str[], unsigned int len){

  uint8_t eui[len];
  char* ptr; //start and end pointer for strtol

  eui[0] = strtol(str, &ptr, HEX );
  for( uint8_t i = 1; i < len; i++ )
  {
    eui[i] = strtol(ptr+1, &ptr, HEX );
  }
  Serial.print(eui[0], HEX);
  for( uint8_t i = 1; i < len; i++)
  {
    Serial.print(':');
    Serial.print( eui[i], HEX);
  }
  return *eui;
}


void lora_init(){
  
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();


    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
    Serial.println(F("Lora Init Done."));
}

void lora_handler(){
 
  os_runloop_once();  
}



void onEvent (ev_t ev) {

    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));

            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}
