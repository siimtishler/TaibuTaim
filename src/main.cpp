#include <Arduino.h>
#include "lightsensor.h"
#include "gauge.h"
#include "soilsensor.h"
#include "humiditysensor.h"
#include "nvsmemory.h"
#include "sensorstask.h"
#include "RTDB.h"
#include "timeutils.h"
#include "bleSerial.h"

#define DEBUG
#include "SerialDebug.h"

light_sens_measurements_t light_measurements = {0};
gauge_measurements_t gauge_measurements = {0};

#include "Wire.h"

RTC_DATA_ATTR int bootcnt = 0;

void trySoil(){
	for (int i = 0; i < 10; i++) {
		uint16_t start = millis();
		initSoilSensor();
		delay(i*30);
		uint16_t sm = getSoilMeasurement();
		DBG("Soil Moisture: "); DBGL(sm);
		Wire.end();
    	delay(1);
		deInitSoilSensor();
		uint16_t end = millis();
		DBG("Time spent Soil: "); DBGL(end-start);
		delay(1000);
	}
}

void setup()
{

	Serial.begin(921600);
	DBGL("");
	DBGL("*******/TERE TAIBUTAIM\\*******");
	DBGL("");
	DBGL("-----------------");
	// trySoil();
	// esp_deep_sleep(10000000);
	memoryInit();
	// If wifi exists, we can send to firebase

	// TODO: Implement that user can change the sleep period from the app
	if(memoryGetWifiExists()) {

		bootcnt++;
		Serial.println("Boot number: " + String(bootcnt));

		// Maximum SSID and password length is 32 and 64 
		// These are 802.11 and WPA2 enforced standards
		char ssid[32];
		char password[64];
		strcpy(ssid, memoryGetSSID().c_str());
		strcpy(password, memoryGetPassword().c_str());

		powerOnDevices();
		mainTask(ssid, password);

		DBGL("Sleeping");
		DBGL("-----------------");
		esp_deep_sleep(MIN_TO_US(10));
	}
	// If not we have to get wifi credentials
	else {
		initBleSerial();
	}
}

void loop()
{
	bleSerialTask();
}
