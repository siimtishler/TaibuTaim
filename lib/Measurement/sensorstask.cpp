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
    delay(10);
}

void sensorsTask() {
    uint32_t last_task_ms = 0;

    light_sens_measurements_t light_measurements = {0};
    gauge_measurements_t gauge_measurements = {0};
    humidity_measurements_t humidity_temperature_measurements = {0};

    uint16_t soil_measurements = 0;

    if(millis() - last_task_ms > SENSORS_TASK_TIMEOUT_MS) {
        
        uint16_t start = millis();
        ConnectWifi();
        uint16_t end = millis();
        DBG("Time spent connect WiFi: "); DBGL(end-start);

        return;

        initSoilSensor();
        soil_measurements = getSoilMeasurement();

        // Light and gauge sensors are SDA->IO7, SCL->IO6
        if(initLightSensor()) {
            light_measurements = getLightMeasurements();
            lightSensorPowerSaverEnable(true);
        }

        if(initGauge()) {
            gaugePowerSaverEnable(false);
            delay(150);
            gauge_measurements = getGaugeMeasurements();
            gaugePowerSaverEnable(true);
        }

        // // SHTC3 Humidity and temp sensor is SDA->IO6, SCL->IO7
        wire_swap();
        // Init performs a software reset, this resets all internal state machines, including sleep mode.
        if(initHumiditySensor()) {
            // humiditySensorSleepEnable(false);
            humidity_temperature_measurements = getHumidityAndTemperatureMeasurements();
            humiditySensorSleepEnable(true);
        }
        wire_swap();

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

        // sendSoilMeasurements(soil_measurements);
        start = millis();        
        DisconnectWifi();
        end = millis();
        DBG("Time spent disconnect: "); DBGL(end-start);

        end = millis();
        DBG("Time spent for all combined: "); DBGL(end-startall);
        last_task_ms = millis();
    }
}
