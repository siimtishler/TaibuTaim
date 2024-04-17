#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "nvsmemory.h"
#include "RTDB.h"
#include "API.h"

#include "timeutils.h"

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define DEBUG
#include "SerialDebug.h"

// These are defined in ignore/API.h
#define WIFI_SSID 					_WIFI_SSID		 // Change into your own WiFi SSID
#define WIFI_PASSWORD 				_WIFI_PASSWORD // Change the password

#define API_KEY 					_API_KEY
#define DATABASE_URL 				_DATABASE_URL

#define BATTERY_SEND_TIMEOUT 		SEC_TO_MS(5)

FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

boolean wifi_connected = false;

bool signupOK = false;

static boolean IsFireBaseReady()
{
	static uint32_t sendDataPrev = 0;
	while (!(Firebase.ready() && signupOK && (millis() - sendDataPrev > 100 || sendDataPrev == 0)))
	{
		sendDataPrev = millis();
	}
	return true;
}

static void SetBatteryDataJSON(gauge_measurements_t measurements, FirebaseJson *pfbjson) {
	pfbjson->add("percentage", String(measurements.percentage));
	pfbjson->add("voltage", String(measurements.voltage));					 
	// pfbjson->add("chgrate", String(measurements.chg_rate));
}

static void SetHumidityDataJSON(humidity_measurements_t measurements, FirebaseJson *pfbjson) {
	pfbjson->add("humidity", String(measurements.humidity));
	pfbjson->add("temperature", String(measurements.temperature));
}

static void SetLightDataJSON(light_sens_measurements_t measurements, FirebaseJson *pfbjson) {
	pfbjson->add("lux", String(measurements.lux));
	pfbjson->add("ambient", String(measurements.ambient));
	pfbjson->add("white", String(measurements.white));
}

static void SetMoistureDataJSON(uint16_t measurements, FirebaseJson *pfbjson) {
	pfbjson->add("moisture", String(measurements));
}


void SetMeasurementsDataJSON(gauge_measurements_t gm, light_sens_measurements_t lm, humidity_measurements_t hm, uint16_t sm, FirebaseJson *pfbjson) {
	FirebaseJson battery;
	SetBatteryDataJSON(gm, &battery);
	pfbjson->add("battery", battery);

	FirebaseJson light;
	SetLightDataJSON(lm, &light);
	pfbjson->add("light", light);

	FirebaseJson humidity;
	SetHumidityDataJSON(hm, &humidity);
	pfbjson->add("humidity", humidity);

	FirebaseJson soil;
	SetMoistureDataJSON(sm, &soil);
	pfbjson->add("soil", soil);

	String str;
	pfbjson->toString(str, true);
	DBGL(str); 
}


static boolean SendWiFiTimestamp()
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.setTimestamp(&fbdo, "wifi-last-connected"))
		{
			DBGL("\nWifi timestamp sent");
			return true;
		}
	}
	return false;
}

static bool PushBatteryData(FirebaseJson fbjson)
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.pushJSON(&fbdo, "battery/", &fbjson))
		{
			if (Firebase.RTDB.setTimestamp(&fbdo, fbdo.dataPath() + "/" + fbdo.pushName() + "/timestamp")){
				String str;
				fbjson.toString(str,true);
				DBGL(str);
				return true;
			}
		}
		DBGL(fbdo.errorReason());
	}
	return false;
}

static bool PushHumidityData(FirebaseJson fbjson)
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.pushJSON(&fbdo, "humidity/", &fbjson))
		{
			if (Firebase.RTDB.setTimestamp(&fbdo, fbdo.dataPath() + "/" + fbdo.pushName() + "/timestamp")){
				String str;
				fbjson.toString(str,true);
				DBGL(str);
				return true;
			}
		}
		DBGL(fbdo.errorReason());
	}
	return false;
}

static bool PushLightData(FirebaseJson fbjson)
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.pushJSON(&fbdo, "light/", &fbjson))
		{
			if (Firebase.RTDB.setTimestamp(&fbdo, fbdo.dataPath() + "/" + fbdo.pushName() + "/timestamp")){
				String str;
				fbjson.toString(str,true);
				DBGL(str);
				return true;
			}
		}
		DBGL(fbdo.errorReason());
	}
	return false;
}

void send(uint16_t val) { 

	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.push(&fbdo, "test/soil", val)){
			if (Firebase.RTDB.pushTimestamp(&fbdo, fbdo.dataPath() + "/" + fbdo.pushName() + "/timestamp")){
				DBG("Sent: ");
				DBGL(val);
			}
		}
	}
}

static bool PushSoilData(FirebaseJson fbjson)
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.pushJSON(&fbdo, "soil/", &fbjson))
		{
			if (Firebase.RTDB.setTimestamp(&fbdo, fbdo.dataPath() + "/" + fbdo.pushName() + "/timestamp")){
				DBGL("Pushed soil moisture");
				return true;
			}
		}
		DBGL(fbdo.errorReason());
	}
	return false;
}

void sendBatteryMeasurements(gauge_measurements_t measurements) {
	FirebaseJson fbjson;
	SetBatteryDataJSON(measurements, &fbjson);
	PushBatteryData(fbjson);
}

void sendHumidityMeasurements(humidity_measurements_t measurements) {
	FirebaseJson fbjson;
	SetHumidityDataJSON(measurements, &fbjson);
	PushHumidityData(fbjson);
}

void sendLightSensorMeasurements(light_sens_measurements_t measurements) {
	FirebaseJson fbjson;
	SetLightDataJSON(measurements, &fbjson);
	PushLightData(fbjson);
}

void sendSoilMeasurements(uint16_t measurements) {
	FirebaseJson fbjson;
	SetMoistureDataJSON(measurements, &fbjson);
	PushSoilData(fbjson);
}

void PushAllData(FirebaseJson fbjson) {
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.pushJSON(&fbdo, "measurements/", &fbjson))
		{
			if (Firebase.RTDB.setTimestamp(&fbdo, fbdo.dataPath() + "/" + fbdo.pushName() + "/timestamp")){
				String str;
				fbjson.toString(str,true);
				DBGL(str);
			}
		}
		DBGL(fbdo.errorReason());
	}
}

void sendAllMeasurements(gauge_measurements_t gm, light_sens_measurements_t lm, humidity_measurements_t hm, uint16_t sm) {
	FirebaseJson fbjson;
	SetMeasurementsDataJSON(gm, lm, hm, sm, &fbjson);
	PushAllData(fbjson);
	sendWiFiStatus();
}


void sendWiFiStatus()
{
	SendWiFiTimestamp(); // Send status

	// wifi_connected not used correctly atm, impl later
	return;

	// Try to reconnect if disconnected
	// if (!wifi_connected)
	// { 
	// 	ConnectWifi();
	// 	ConnectFirebase();
	// }
	// else
	// {
	// 	SendWiFiTimestamp(); // Send status
	// }

}

std::vector<std::string> getAllWiFiSSIDs() {
	uint8_t num = WiFi.scanNetworks();
	std::vector<std::string> ssid;
	if(num == 0) {
		Serial.println("no networks");
		return ssid;
	}

	ssid.push_back("<SSID_START>");

	for (int i = 0; i < num; i++)
	{
		ssid.push_back(WiFi.SSID(i).c_str());
	}
	ssid.push_back("</SSID_END>");

	return ssid;
}


const uint8_t* convertCharToUint8Array(const char* chr) {
	static uint8_t octets[4]; // Static array to hold the octets
    
    // Use sscanf to parse the IP address string
    sscanf(chr, "%hhu.%hhu.%hhu.%hhu", &octets[0], &octets[1], &octets[2], &octets[3]);

    return octets; // Return the pointer to the array
}

RTC_DATA_ATTR boolean badConfig = false;

void setbadConfig(boolean val) {
	badConfig = val;
}

boolean getbadConfig() {
	return badConfig;
}

boolean ConnectWifi(const char* ssid, const char* password, boolean tryConnectFirstTime)
{
	boolean wifiExistsInMemory = memoryGetWifiExists();

	if(strlen(ssid) == 0) {
		if(wifiExistsInMemory) {
			memorySetWifiExists(false);
		}
		DBGL("Missing SSID");
		return false;
	}
	WiFi.persistent(false);


	if(wifiExistsInMemory && !tryConnectFirstTime && !badConfig) {
		DBGL("Taking Wifi config from memory");
		char ip[16]; 
		char gw[16]; 
		char sn[16]; 
		char dn[16]; 

		strcpy(ip, memoryGetLocalIP().c_str());
		strcpy(gw, memoryGetGatewayIP().c_str());
		strcpy(sn, memoryGetSubnetIP().c_str());
		strcpy(dn, memoryGetDnsIP().c_str());

		IPAddress local = IPAddress(convertCharToUint8Array(ip));
		IPAddress gateway = IPAddress(convertCharToUint8Array(gw));
		IPAddress subnet = IPAddress(convertCharToUint8Array(sn));
		IPAddress dns = IPAddress(convertCharToUint8Array(dn));

		// DBGL(local.toString());
		// DBGL(gateway.toString());
		// DBGL(subnet.toString());
		// DBGL(dns.toString());

		WiFi.config(local, gateway, subnet, dns);
		WiFi.mode(WIFI_STA);
		// ssid and password are previousl taken from memory when wifiExistsInMemory is true
		WiFi.begin(ssid, password);
	}

	else {
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, password);
	}

	DBGL("Connecting to Wi-Fi");
	unsigned long startMillis = millis();

	while (WiFi.status() != WL_CONNECTED && millis() - startMillis < 5000)
	{
		DBG(".");
		delay(100);
	}
	DBGL("");
	
	if (WiFi.status() == WL_CONNECTED)
	{

		if (!wifiExistsInMemory)
		{
			memorySetLocalIP(WiFi.localIP().toString());
			memorySetDnsIP(WiFi.dnsIP().toString());
			memorySetGatewayIP(WiFi.gatewayIP().toString());
			memorySetSubnetIP(WiFi.subnetMask().toString());
			memorySetSSID(String(ssid));
			memorySetPassword(String(password));

			memorySetWifiExists(true);
		}
		wifi_connected = true;
	}
	else // If connection failed and timeout reached
	{
		if(wifiExistsInMemory) {
			memorySetWifiExists(false);
		}
		DBGL("\nWi-Fi connection failed. Timeout reached.");
		wifi_connected = false;
		// Handle the failure or retry if needed
	}
	return wifi_connected;
}

void DisconnectWifi() {
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
}

// Creates Firebase connection
void ConnectFirebase()
{
	if (!wifi_connected)
		return;
	/* Assign the api key (required) */
	config.api_key = API_KEY;

	auth.user.email = "esp1@taibutaim.ee";
	auth.user.password = "123456";
	config.database_url = DATABASE_URL;

	// if (Firebase.signUp(&config, &auth, auth.user.email, auth.user.password))
	// {
	// 	Serial.println("Signup OK");
	// 	signupOK = true;
	// }
	// else
	// {
	// 	Serial.printf("%s\n", config.signer.signupError.message.c_str());
	// }
	/* Assign the RTDB URL (required) */

	Firebase.reconnectNetwork(true);
	fbdo.setResponseSize(4096);

	/* Assign the callback function for the long running token generation task */
	config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

	signupOK = true;

	Firebase.begin(&config, &auth);
	// Firebase.reconnectNetwork(true);
}