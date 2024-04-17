#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <vector>
#include "lightsensor.h"
#include "gauge.h"
#include "humiditysensor.h"

boolean ConnectWifi(const char* ssid = "", const char* password = "", boolean tryConnectFirstTime = false);
void DisconnectWifi();

void ConnectFirebase();

std::vector<std::string> getAllWiFiSSIDs();

void sendBatteryMeasurements(gauge_measurements_t measurements);
void sendLightSensorMeasurements(light_sens_measurements_t measurements);
void sendHumidityMeasurements(humidity_measurements_t measurements);
void sendSoilMeasurements(uint16_t measurements);

void sendAllMeasurements(gauge_measurements_t gm, light_sens_measurements_t lm, humidity_measurements_t hm, uint16_t sm);

void send(uint16_t val);

void sendWiFiStatus();

void setbadConfig(boolean val);
boolean getbadConfig();

extern boolean wifi_connected;

