#pragma once

#include <Arduino.h>
#include <Preferences.h>

extern Preferences pref;

void memoryInit();
void memoryEnd();

String memoryGetString(const char* key);
size_t memorySetString(const char* key, const char* value);

boolean memoryGetBool(const char* key);
size_t memorySetBool(const char* key, boolean value);

String memoryGetSSID();
void memorySetSSID(const String& ssid);

String memoryGetPassword();
void memorySetPassword(const String& password);

String memoryGetLocalIP();
void memorySetLocalIP(const String& localIP);

String memoryGetGatewayIP();
void memorySetGatewayIP(const String& gatewayIP);

String memoryGetSubnetIP();
void memorySetSubnetIP(const String& subnetIP);

String memoryGetDnsIP();
void memorySetDnsIP(const String& dnsIP);

boolean memoryGetWifiExists();
void memorySetWifiExists(boolean wifiExists);

