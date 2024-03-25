#pragma once
#include <stdint.h>
#include <Arduino.h>
#include "lightsensor.h"
#include "gauge.h"
#include "humiditysensor.h"

void ConnectWifi();
void ConnectFirebase();

void DisconnectWifi();

void sendBatteryMeasurements(gauge_measurements_t measurements);
void sendLightSensorMeasurements(light_sens_measurements_t measurements);
void sendHumidityMeasurements(humidity_measurements_t measurements);
void sendSoilMeasurements(uint16_t measurements);

void SendDeviceMeasurements();

extern boolean wifi_connected;