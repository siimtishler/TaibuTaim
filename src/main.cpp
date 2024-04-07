// #include <Arduino.h>
// #include "lightsensor.h"
// #include "gauge.h"
// #include "soilsensor.h"
// #include "humiditysensor.h"

// #include "sensorstask.h"

// #include "RTDB.h"

// #include "timeutils.h"

// #define DEBUG
// #include "SerialDebug.h"

// light_sens_measurements_t light_measurements = {0};
// gauge_measurements_t gauge_measurements = {0};

// #include "Wire.h"

// RTC_DATA_ATTR int bootcnt = 0;

// RTC_DATA_ATTR boolean gaugePowerOn = false;
// RTC_DATA_ATTR boolean humiditySensPowerOn = false;
// RTC_DATA_ATTR boolean lightSensPowerOn = false;

// void powerOnDevices() {

// 	if(!gaugePowerOn) {
// 		if(powerOnGauge()){
// 			// DBGL("Powering gauge");
// 			gaugePowerOn = true;
// 		}
// 	}

// 	if(!lightSensPowerOn) {
// 		if(powerOnLightSensor()){
// 			// DBGL("Powering light sensor");
// 			lightSensPowerOn = true;
// 		}
// 	}

// 	if(!humiditySensPowerOn) {
// 		// Have to stop I2C interface for rewiring SDA and SCL for humidity sens
// 		Wire.end();
// 		delay(1);
// 		if(powerOnHumiditySensor()) {
// 			// DBGL("Powering humidity sensor");
// 			humiditySensPowerOn = true;
// 		}
// 	}
// }

// void setup()
// {

// 	Serial.begin(921600);
// 	DBGL("*******/TERE TAIBUTAIM\\*******");
// 	DBGL("");
// 	DBGL("-----------------");
// 	bootcnt++;
// 	Serial.println("Boot number: " + String(bootcnt));

// 	// powerOnDevices();

// 	// initSoilSensor();

// 	// sensorsTask();

// 	// ConnectWifi();
// 	// ConnectFirebase();

// 	// if(initLightSensor()) {
// 	// 	lightSensorPowerSaverEnable(false);
// 	// 	light_measurements = getLightMeasurements();
// 	// 	lightSensorPowerSaverEnable(true);
// 	// }
// 	// send(getLightMeasurements(), "sitt/");

// 	initBleSerial();
// 	DBGL("Sleeping");
// 	DBG("-----------------");
// 	// esp_deep_sleep(SEC_TO_US(10));
// }

#include "WiFi.h"
#include "RTDB.h"
#include "bleserial.h"

#include <Arduino.h>


void setup()
{
	Serial.begin(921600);


	
	initBleSerial();
}



void loop()
{
	// TODO: When WiFi exists in non volatile memory, dont start BLESerial.
	bleSerialTask();
}