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

static inline void wire_swap() {
    Wire.end();
    delay(1);
}


static void measureTimeTakenForTask(light_sens_measurements_t lm, gauge_measurements_t gm, humidity_measurements_t hm, uint16_t sm) {
    uint16_t startall = millis();
    uint16_t start = 0;
    uint16_t end = 0;

    start = millis();
    ConnectWifi();
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

void sensorsTask(const char *ssid, const char *password) {

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

#ifdef MEASURE_TIME
    measureTimeTakenForTask(light_measurements, gauge_measurements, humidity_temperature_measurements, soil_measurements);
#else
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
