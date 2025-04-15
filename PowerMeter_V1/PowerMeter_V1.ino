#include "SimpleModbusMaster.h"

#define connection_error_led 12

#define baud 9600
#define timeout 1000
#define polling 200 

#define retry_count 10
#define TxEnablePin 0

// 1. PERINTAH UNTUK MENAMBAHKAN PEMBACAAN MODBUS
enum {
    PACKET1,
    PACKET2,
    TOTAL_NO_OF_PACKETS
};

Packet packets[TOTAL_NO_OF_PACKETS];

// 2. PERINTAH UNTUK MENAMBAHKAN PEMBACAAN MODBUS
packetPointer packet1 = &packets[PACKET1];
unsigned int tegangan[2];
packetPointer packet2 = &packets[PACKET2];
unsigned int teganganAVG[2];

float f_2uint_float(uint16_t msb, uint16_t lsb) {
  union {
    uint32_t i;
    float f;
  } u;

  u.i = ((uint32_t)msb << 16) | lsb;
  return u.f;
}

unsigned long last_toggle = 0;
unsigned long perviousMillisPrint = 0;

void setup()
{
    Serial.begin(9600);
    Serial.println("Run System Power Monitoring V1.0");
    delay(3000);

    // 3. PERINTAH UNTUK MENAMBAHKAN PEMBACAAN MODBUS
    packet1->id = 1; // EDRESS MODBUS ID UNIT MODBUS
    packet1->function = READ_HOLDING_REGISTERS;
    packet1->address = 3027; // REGISTER YANG ADA PADA DATA SHEET
    packet1->no_of_registers = 2; // MEMBACA 2 REGISTER = 3028 DAN 3029(AUTO)
    packet1->register_array = tegangan;

    packet2->id = 1; // EDRESS MODBUS ID UNIT MODBUS
    packet2->function = READ_HOLDING_REGISTERS;
    packet2->address = 3035; // REGISTER YANG ADA PADA DATA SHEET
    packet2->no_of_registers = 2; // MEMBACA 2 REGISTER = 3028 DAN 3029(AUTO)
    packet2->register_array = teganganAVG;

    
    modbus_configure(baud, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
    pinMode(connection_error_led, OUTPUT);
}

void loop()
{
  unsigned int connection_status = modbus_update(packets);

  if (connection_status != TOTAL_NO_OF_PACKETS) {
      digitalWrite(connection_error_led, HIGH);
  } else {
      digitalWrite(connection_error_led, LOW);
  }
  long timerPrint = millis();
  if(timerPrint -  perviousMillisPrint >= 1000){
    Serial.print("Tegangan           : "); Serial.println(f_2uint_float(tegangan[0], tegangan[1]));
    Serial.print("Tegangan Rata-Rata : "); Serial.println(f_2uint_float(teganganAVG[0], teganganAVG[1]));
    perviousMillisPrint = timerPrint;
  }
}
