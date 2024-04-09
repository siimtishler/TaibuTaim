#include "sensorstask.h"
// Inlcude all the separate sensors
#include "lightsensor.h"
#include "gauge.h"
#include "humiditysensor.h"
#include "soilsensor.h"
#include "Wire.h"

#include "RTDB.h"

#include "RTDB.h"

#define DEBUG
#include "SerialDebug.h"

#include "timeutils.h"

#define SENSORS_TASK_TIMEOUT_MS     SEC_TO_MS(0)

static inline void wire_swap() {
    Wire.end();
    delay(1);
}

// #define MEASURE_TIME

void sensorsTask() {
    uint32_t last_task_ms = 0;

    light_sens_measurements_t light_measurements = {0};
    gauge_measurements_t gauge_measurements = {0};
    humidity_measurements_t humidity_temperature_measurements = {0};

    uint16_t soil_measurements = 0;

    if(millis() - last_task_ms > SENSORS_TASK_TIMEOUT_MS) {

        uint16_t start = 0;
        uint16_t end = 0;

        // initSoilSensor();
        soil_measurements = getSoilMeasurement();
        wire_swap();

        // Light and gauge sensors are SDA->IO7, SCL->IO6
        if(initLightSensor()) {
            light_measurements = getLightMeasurements();
            // When we first give power, we turn on powersaver
            // lightSensorPowerSaverEnable(true);
        }

        if(initGauge()) {
            // gaugeHibernate(false);
            gauge_measurements = getGaugeMeasurements();
            // delay(10);
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
        uint16_t startall = millis();

        start = millis();
        ConnectWifi();
        end = millis();
        DBG("Time spent connect WiFi: "); DBGL(end-start);

        start = millis();
        ConnectFirebase();
        end = millis();
        DBG("Time spent connect FireBase: "); DBGL(end-start);

        start = millis();
        sendHumidityMeasurements(humidity_temperature_measurements);
        end = millis();
        DBG("Time spent humidity: "); DBGL(end-start);

        start = millis();
        sendBatteryMeasurements(gauge_measurements);
        end = millis();
        DBG("Time spent batt: "); DBGL(end-start);

        start = millis();
        sendLightSensorMeasurements(light_measurements);
        end = millis();
        DBG("Time spent light: "); DBGL(end-start);

        start = millis();        
        DisconnectWifi();
        end = millis();
        DBG("Time spent disconnect: "); DBGL(end-start);

        end = millis();
        DBG("Time spent for all combined: "); DBGL(end-startall);
        last_task_ms = millis();
    #else
        ConnectWifi("CGA2121_2.4GHZ", "kell1234");
        ConnectFirebase();
        // sendHumidityMeasurements(humidity_temperature_measurements);
        // // sendBatteryMeasurements(gauge_measurements);
        // sendLightSensorMeasurements(light_measurements);
        // sendSoilMeasurements(soil_measurements);

        sendAllMeasurements(gauge_measurements, light_measurements, humidity_temperature_measurements, soil_measurements);
        DisconnectWifi();
    #endif
    }
}
