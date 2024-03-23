#include "soilsensor.h"

#define DEBUG
#include "SerialDebug.h"

#define SOIL_SENSOR_PIN         4

boolean initSoilSensor() {
    pinMode(SOIL_SENSOR_PIN, INPUT);
    DBGL("Soil sensor init success");
    return true;
}   

uint16_t getSoilMeasurement() {
    uint16_t measurement = 0;
    uint8_t measurement_cycles = 10;

    for (int i = 0; i < measurement_cycles; i++) {
        measurement += analogRead(SOIL_SENSOR_PIN);
    }

    uint16_t avg_measurement = measurement / measurement_cycles;
    DBGL(avg_measurement);
    return avg_measurement;
}