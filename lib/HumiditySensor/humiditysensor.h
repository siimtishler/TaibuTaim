#pragma once
#include <Arduino.h>

typedef struct{
    float humidity;
    float temperature;
} humidity_measurements_t;


boolean initHumiditySensor();
boolean powerOnHumiditySensor();
void humiditySensorSleepEnable(boolean);

float getLastMeasuredTemperature();
float getLastMeasuredHumidity();
void measureHumidityAndTemp();
humidity_measurements_t getHumidityAndTemperatureMeasurements();