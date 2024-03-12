#include "lightsensor.h"
#include "Adafruit_VEML7700.h"
#include "timeutils.h"

#define DEBUG
#include "SerialDebug.h"

#define LIGHT_SENSOR_TIMEOUT SEC_TO_MS(2)

Adafruit_VEML7700 sensor = Adafruit_VEML7700();

uint16_t getLux() {
    return (uint16_t)sensor.readLux(VEML_LUX_AUTO);
}

uint16_t getWhite() {
    return sensor.readWhite();
}

uint16_t getAmbient() {
    return sensor.readALS();
}

boolean initLightSensor(){
    if(sensor.begin()) {
        DBGL("Light sensor inited");
        return true;
    }
    return false;
}

void printLightMeasurements(light_sens_measurements_t* measurements){
    DBG("Lux: ");
    DBGL(measurements->lux);
    DBG("White: ");
    DBGL(measurements->white);
    DBG("Ambient: ");
    DBGL(measurements->ambient);
}

void lightSensorTask(light_sens_measurements_t* measurements) {
    static uint32_t last_task_ms = 0;
    if(millis() - last_task_ms > LIGHT_SENSOR_TIMEOUT){

        measurements->lux = getLux();
        measurements->white = getWhite();
        measurements->ambient = getAmbient();

        // printLightMeasurements(measurements);

        last_task_ms = millis();
    }
}