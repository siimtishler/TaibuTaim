#pragma once
#include <Arduino.h>

typedef struct{
    float humidity;
    float temperature;
} humidity_measurements_t;


boolean initHumiditySensor();

float getLastMeasuredTemperature();
float getLastMeasuredHumidity();
void measureHumidityAndTemp();
humidity_measurements_t getHumidityAndTemperatureData();