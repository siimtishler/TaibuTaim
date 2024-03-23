#include "sensorstask.h"
// Inlcude all the separate sensors
#include "lightsensor.h"
#include "gauge.h"
#include "humiditysensor.h"
#include "soilsensor.h"
#include "Wire.h"

#include "RTDB.h"

#define DEBUG
#include "SerialDebug.h"

#include "timeutils.h"

#define SENSORS_TASK_TIMEOUT_MS     SEC_TO_MS(5)

static inline void wire_swap() {
    Wire.end();
    delay(10);
}

void sensorsTask() {
    static uint32_t last_task_ms = 0;

    static boolean soil_sensors_init = false;

    static light_sens_measurements_t light_data = {0};
    static gauge_measurements_t gauge_data = {0};
    static humidity_measurements_t humidity_temperature_data = {0};

    static uint16_t soil_data = 0;

    if(millis() - last_task_ms > SENSORS_TASK_TIMEOUT_MS) {
        
        // if(!soil_sensors_init){
        //     initSoilSensor();
        //     soil_sensors_init = true;
        // }
        // else {
        //     soil_data = getSoilMeasurement();
        // }

        // Light and gauge sensors are SDA->IO7, SCL->IO6
        // if(initLightSensor()){
        //     light_data = getLightData();
        // }

        if(initGauge()) {
            delay(750);
            gauge_data = getGaugeData();
        }

        // // SHTC3 Humidity and temp sensor is SDA->IO6, SCL->IO7
        // wire_swap();
        // if(initHumiditySensor()){
        //     humidity_temperature_data = getHumidityAndTemperatureData();
        // }
        // wire_swap();

        sendBatteryStatus(gauge_data);


        last_task_ms = millis();
    }
}
