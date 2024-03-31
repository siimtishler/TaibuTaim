#pragma once
#include <Arduino.h>

typedef struct{
    uint16_t lux;
    uint16_t white;
    uint16_t ambient;
} light_sens_measurements_t;

boolean initLightSensor();
boolean powerOnLightSensor();

void lightSensorPowerSaverEnable(boolean);

uint16_t getLux();
uint16_t getWhite();
uint16_t getAmbient();

void printLightMeasurements(light_sens_measurements_t*);
light_sens_measurements_t getLightMeasurements();
void lightSensorTask(light_sens_measurements_t*);
