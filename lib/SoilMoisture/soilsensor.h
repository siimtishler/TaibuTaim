#pragma once
#include <Arduino.h>

boolean initSoilSensor();
uint16_t getSoilMeasurement();
boolean deInitSoilSensor();