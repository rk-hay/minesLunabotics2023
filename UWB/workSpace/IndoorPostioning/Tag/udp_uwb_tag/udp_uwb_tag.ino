/*

For ESP32 UWB or ESP32 UWB Pro

*/

#include <SPI.h>
#include <DW1000Ranging.h>
#include <WiFi.h>
#include "link.h"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4
#define PIN_RST 27
#define PIN_IRQ 34

void setup()
{
    Serial.begin(115200);


    delay(1000);

    //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, DW_CS, PIN_IRQ);
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);

    //we start the module as a tag
    DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_LOWPOWER);

}

void loop()
{
    DW1000Ranging.loop();

}

void newRange()
{
    char c[30];

    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
}

void newDevice(DW1000Device *device)
{
    Serial.print("ranging init; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
}

//------------------------------------------------------------------------------------------------------------------------------------------------

int8_t checkReceiver() {
  byte forWho = data[16];
  byte fromWhom = data[17];

  if (forWho != MY_NUM) {
    //    Serial.print('n');
    return 1;
  } else if ( ( OTHER_NUM == 0 ) && ( expectedMsgId == POLL) ) { // let's talk to a new localino
    OTHER_NUM = fromWhom;
    return 0; 
  } else if (fromWhom != OTHER_NUM) {
    Serial.print('N');
    Serial.print(OTHER_NUM);
    Serial.print(fromWhom);
    return 2;
  }
  else {
    Serial.print('G');
    return 0;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------

