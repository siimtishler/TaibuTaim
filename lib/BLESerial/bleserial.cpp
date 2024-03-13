#include "bleserial.h"
#include "BluetoothSerial.h"

BluetoothSerial bleSerial;

void initBleSerial(){
    bleSerial.begin(String("TaibuTaim #1"));
}

void bleSerialTask(){
    if (Serial.available())
    {
        bleSerial.write(Serial.read());
    }
    if (bleSerial.available())
    {
        Serial.write(bleSerial.read());
    }
    delay(20);
}
