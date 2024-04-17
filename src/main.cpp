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

RTC_DATA_ATTR boolean gaugePowerOn = false;
RTC_DATA_ATTR boolean humiditySensPowerOn = false;
RTC_DATA_ATTR boolean lightSensPowerOn = false;

void powerOnDevices() {

	if(!gaugePowerOn) {
		if(powerOnGauge()){
			// DBGL("Powering gauge");
			gaugePowerOn = true;
		}
	}

	if(!lightSensPowerOn) {
		if(powerOnLightSensor()){
			// DBGL("Powering light sensor");
			lightSensPowerOn = true;
		}
	}

	if(!humiditySensPowerOn) {
		// Have to stop I2C interface for rewiring SDA and SCL for humidity sens
		Wire.end();
		delay(1);
		if(powerOnHumiditySensor()) {
			// DBGL("Powering humidity sensor");
			humiditySensPowerOn = true;
		}
	}
}

void setup()
{

	Serial.begin(921600);
	DBGL("");
	DBGL("*******/TERE TAIBUTAIM\\*******");
	DBGL("");
	DBGL("-----------------");
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
		sensorsTask(ssid, password);

		// if(ConnectWifi(memoryGetSSID().c_str(), memoryGetPassword().c_str())) {
		// 	DBGL("Connecting to fb and stuff");
		// 	ConnectFirebase();
		// 	sendWiFiStatus();
		// }
		// else {
		// 	esp_restart();
		// }
		// sendWiFiStatus();

		DBGL("Sleeping");
		DBGL("-----------------");
		esp_deep_sleep(MIN_TO_US(10));
	}
	// If not we have to get wifi credentials
	else {
		initBleSerial();
	}

	// ConnectWifi("CGA2121_2.4GHZ", "kell1234");
	// ConnectFirebase();
	// send(13);



}

void loop()
{
	bleSerialTask();
}
