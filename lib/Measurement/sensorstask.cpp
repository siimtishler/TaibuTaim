#include "sensorstask.h"
// Inlcude all the separate sensors
#include "lightsensor.h"
#include "gauge.h"
#include "humiditysensor.h"
#include "soilsensor.h"
#include "Wire.h"
#include "RTDB.h"

#include "timeutils.h"

#define DEBUG
#include "SerialDebug.h"
// #define MEASURE_TIME

#define SENSORS_TASK_TIMEOUT_MS     SEC_TO_MS(0)


RTC_DATA_ATTR boolean gaugePowerOn = false;
RTC_DATA_ATTR boolean humiditySensPowerOn = false;
RTC_DATA_ATTR boolean lightSensPowerOn = false;

static inline void wire_swap() {
    Wire.end();
    delay(1);
}


static void measureTimeTakenForTask(const char *ssid, const char *password) {
    uint16_t startall = millis();
    uint16_t start = 0;
    uint16_t end = 0;
    light_sens_measurements_t lm;
    gauge_measurements_t gm;
    humidity_measurements_t hm;
    uint16_t sm;

    start = millis();
    initSoilSensor();
    sm = getSoilMeasurement();
    wire_swap();
    deInitSoilSensor();
    end = millis();
    DBG("Time spent Soil: "); DBGL(end-start);

    start = millis();
    if(initLightSensor()) {
        lm = getLightMeasurements();
        // When we first give power, we turn on powersaver
        // lightSensorPowerSaverEnable(true);
    }
    end = millis();
    DBG("Time spent Light: "); DBGL(end-start);

    start = millis();
    if(initGauge()) {
        gm = getGaugeMeasurements();
        delay(1);
        gaugeHibernate(true);
    }
    end = millis();
    DBG("Time spent Gauge: "); DBGL(end-start);

    
    start = millis();
    wire_swap();
    // Init performs a software reset, this resets all internal state machines, including sleep mode.
    if(initHumiditySensor()) {
        humiditySensorSleepEnable(false);
        hm = getHumidityAndTemperatureMeasurements();
        humiditySensorSleepEnable(true);
    }
    end = millis();
    DBG("Time spent humidity: "); DBGL(end-start);


    start = millis();
    ConnectWifi(ssid, password);
    end = millis();
    DBG("Time spent connect WiFi: "); DBGL(end-start);

    start = millis();
    ConnectFirebase();
    end = millis();
    DBG("Time spent connect FireBase: "); DBGL(end-start);

    start = millis();
    sendAllMeasurements(gm, lm, hm, sm);
    end = millis();
    DBG("Time spent send all measurements: "); DBGL(end-start);

    start = millis();        
    DisconnectWifi();
    end = millis();
    DBG("Time spent disconnect: "); DBGL(end-start);

    end = millis();
    DBG("Time spent for all combined: "); DBGL(end-startall);
}

void mainTask(const char *ssid, const char *password) {

#ifdef MEASURE_TIME
    measureTimeTakenForTask(ssid, password);
#else

    light_sens_measurements_t light_measurements = {0};
    gauge_measurements_t gauge_measurements = {0};
    humidity_measurements_t humidity_temperature_measurements = {0};

    uint16_t soil_measurements = 0;

    initSoilSensor();
    soil_measurements = getSoilMeasurement();
    wire_swap();
    deInitSoilSensor();

    // Light and gauge sensors are SDA->IO7, SCL->IO6
    if(initLightSensor()) {
        light_measurements = getLightMeasurements();
        // When we first give power, we turn on powersaver
        // lightSensorPowerSaverEnable(true);
    }

    if(initGauge()) {
        gauge_measurements = getGaugeMeasurements();
        delay(1);
        gaugeHibernate(true);
    }

    // // SHTC3 Humidity and temp sensor is SDA->IO6, SCL->IO7
    wire_swap();
    // Init performs a software reset, this resets all internal state machines, including sleep mode.
    if(initHumiditySensor()) {
        humiditySensorSleepEnable(false);
        humidity_temperature_measurements = getHumidityAndTemperatureMeasurements();
        humiditySensorSleepEnable(true);
    }


    if (ConnectWifi(ssid, password)) {
        ConnectFirebase();
        sendAllMeasurements(gauge_measurements, light_measurements, humidity_temperature_measurements, soil_measurements);
        sendWiFiStatus();
        DisconnectWifi();
    }
    else {
        esp_restart();
    }
#endif
}

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
