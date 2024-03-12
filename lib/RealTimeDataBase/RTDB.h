#pragma once
#include <stdint.h>
#include <Arduino.h>
#include "lightsensor.h"
#include "gauge.h"

void ConnectWifi();
void ConnectFirebase();

void sendBatteryStatus(gauge_measurements_t measurements);
void sendLightSensorStatus(light_sens_measurements_t measurements);

void SendDeviceStatuses();

extern boolean wifi_connected;