#include "nvsmemory.h"

// #define DEBUG
#include "SerialDebug.h"

Preferences pref;

void memoryInit() {
    DBGL("Memory init");
    pref.begin("wifi", false);
}

void memoryEnd() {
    pref.end();
}

String memoryGetString(const char* key) {
    return pref.getString(key, "");
}

size_t memorySetString(const char* key, const char* value) {
    return pref.putString(key, value);
}

boolean memoryGetBool(const char* key) {
    return pref.getBool(key, false);
}

size_t memorySetBool(const char* key, boolean value) {
    return pref.putBool(key, value);
}

// Specific wifi config getters/setters

String memoryGetLocalIP() {
    return pref.getString("local_ip", "");
}

void memorySetLocalIP(const String& localIP) {
    DBGL("Setting local IP: " + localIP);
    pref.putString("local_ip", localIP);
}

String memoryGetSSID() {
    return pref.getString("ssid", "");
}

void memorySetSSID(const String& ssid) {
    DBGL("Setting SSID: " + ssid);
    pref.putString("ssid", ssid);
}

String memoryGetPassword() {
    return pref.getString("password", "");
}

void memorySetPassword(const String& password) {
    DBGL("Setting password: " + password);
    pref.putString("password", password);
}

boolean memoryGetWifiExists() {
    return pref.getBool("wifi_exists", false);
}

void memorySetWifiExists(boolean wifiExists) {
    DBGL("Setting wifi exists: " + String(wifiExists));
    pref.putBool("wifi_exists", wifiExists);
}

String memoryGetGatewayIP() {
    return pref.getString("gatewayIP", "");
}

void memorySetGatewayIP(const String& gatewayIP) {
    DBGL("Setting gateway IP: " + gatewayIP);
    pref.putString("gatewayIP", gatewayIP);
}

String memoryGetSubnetIP() {
    return pref.getString("subnetIP", "");
}

void memorySetSubnetIP(const String& subnetIP) {
    DBGL("Setting subnet IP: " + subnetIP);
    pref.putString("subnetIP", subnetIP);
}

String memoryGetDnsIP() {
    return pref.getString("dnsIP", "");
}

void memorySetDnsIP(const String& dnsIP) {
    DBGL("Setting dns IP: " + dnsIP);
    pref.putString("dnsIP", dnsIP);
}
