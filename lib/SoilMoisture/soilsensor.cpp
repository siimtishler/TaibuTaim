#include "soilsensor.h"

// #define DEBUG
#include "SerialDebug.h"

#define SOIL_SENSOR_PIN         4
#define CMOS_TIMER_POWER_PIN    1

boolean initSoilSensor() {
    pinMode(CMOS_TIMER_POWER_PIN, OUTPUT);
    digitalWrite(CMOS_TIMER_POWER_PIN, HIGH);
    pinMode(SOIL_SENSOR_PIN, INPUT);
    delay(200);
    DBGL("Soil sensor init success");
    return true;
}   

boolean deInitSoilSensor() {
    digitalWrite(CMOS_TIMER_POWER_PIN, LOW);
    pinMode(CMOS_TIMER_POWER_PIN, INPUT);
    DBGL("Soil sensor deinit success");
    return true;
}

uint16_t getSoilMeasurement() {
    uint16_t measurement = 0;
    uint8_t measurement_cycles = 10;
    for (int i = 0; i < measurement_cycles; i++) {
        delay(1);
        measurement += analogRead(SOIL_SENSOR_PIN);
        // Serial.println(measurement);
    }

    uint16_t avg_measurement = measurement / measurement_cycles;
    DBG("SOIL MOISTURE ");DBGL(avg_measurement);
    return avg_measurement;
}