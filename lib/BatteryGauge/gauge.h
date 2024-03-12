#pragma once
#include <Arduino.h>

typedef struct{
    float voltage;
    float percentage;
    float chg_rate;
} gauge_measurements_t;

boolean initGauge();

float getCellVoltage();
float getPercentage();
float getChgRate();

void printGaugeMeasurements(gauge_measurements_t* );
void gaugeTask(gauge_measurements_t*);
