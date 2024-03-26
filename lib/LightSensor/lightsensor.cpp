#include "lightsensor.h"
#include "Adafruit_VEML7700.h"
#include "timeutils.h"

// #define DEBUG
#include "SerialDebug.h"

#define LIGHT_SENSOR_TIMEOUT SEC_TO_MS(2)

#define LIGHT_SDA_PIN       7
#define LIGHT_SCL_PIN       6

Adafruit_VEML7700 lightsensor = Adafruit_VEML7700();

uint16_t getLux() {
    return (uint16_t)lightsensor.readLux(VEML_LUX_AUTO);
}

uint16_t getWhite() {
    return lightsensor.readWhite();
}

uint16_t getAmbient() {
    return lightsensor.readALS();
}

boolean initLightSensor(){
    for (int i = 0; i < 10; i++)
    {
        if(lightsensor.begin(&Wire, LIGHT_SDA_PIN, LIGHT_SCL_PIN)) {
            DBGL("Light sensor inited");
            return true;
        }
    }
    return false;
}

void lightSensorPowerSaverEnable(boolean enable) {
    if(enable) {
        if(!lightsensor.powerSaveEnabled()){
            DBGL("Enabling power saver");
            lightsensor.enable(false);
            lightsensor.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);
            lightsensor.enable(true);
            return;
        }
        DBGL("Light sensor power saver already on");
        return;
    }
    else {
        if(lightsensor.powerSaveEnabled()){
            DBGL("Disabling power saver");
            lightsensor.enable(false);
            lightsensor.powerSaveEnable(false);
            lightsensor.enable(true);
            return;
        }
        DBGL("Light sensor power saver already off");
        return;
    }

}

void printLightMeasurements(light_sens_measurements_t* measurements){
    DBG("Lux: ");
    DBGL(measurements->lux);
    DBG("White: ");
    DBGL(measurements->white);
    DBG("Ambient: ");
    DBGL(measurements->ambient);
}

light_sens_measurements_t getLightMeasurements() {
    light_sens_measurements_t measurements = {0};

    measurements.lux = getLux();
    measurements.white = getWhite();
    measurements.ambient = getAmbient();

#ifdef DEBUG
    printLightMeasurements(&measurements);
#endif

    return measurements;
}

void lightSensorTask(light_sens_measurements_t* measurements) {
    static uint32_t last_task_ms = 0;
    if(millis() - last_task_ms > LIGHT_SENSOR_TIMEOUT){


        printLightMeasurements(measurements);

        last_task_ms = millis();
    }
}