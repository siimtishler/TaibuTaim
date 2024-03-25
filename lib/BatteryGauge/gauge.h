#pragma once
#include <Arduino.h>

typedef struct{
    float voltage;
    float percentage;
    float chg_rate;
} gauge_measurements_t;

boolean initGauge();
boolean powerOnGauge();
void gaugePowerSaverEnable(boolean);

float getCellVoltage();
float getPercentage();
float getChgRate();

void printGaugeMeasurements(gauge_measurements_t* );
gauge_measurements_t getGaugeMeasurements();
void gaugeTask(gauge_measurements_t*);
