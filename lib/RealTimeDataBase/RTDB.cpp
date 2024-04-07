#include <WiFi.h>
#include <Firebase_ESP_Client.h>
// #include <ArduinoJson.h>
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
	pfbjson->add("chgrate", String(measurements.chg_rate));
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

static void SendWiFiTimestamp()
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.setTimestamp(&fbdo, "lockers/locker1/wifi-last-connected"))
		{
			DBGL("\nWifi timestamp sent");
		}
	}
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

void send(light_sens_measurements_t val, const char* path) { 
	FirebaseJson fbj;
	fbj.add("A", val.ambient);
	fbj.add("L", val.lux);
	fbj.add("W", val.white);
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.setJSON(&fbdo, path, &fbj)){
			DBGL("Sent");
		}
	}
}

static bool PushSoilData(uint16_t soilMoisture)
{
	if (IsFireBaseReady())
	{
		String str_soilMoisture = String(soilMoisture);
		if (Firebase.RTDB.pushInt(&fbdo, "soil_moisture/", soilMoisture))
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
	PushSoilData(measurements);
}


static void SendWiFiStatus()
{
	static uint32_t wifi_last_status_sent = 0;

	if ((millis() - wifi_last_status_sent) > 5000)
	{ // Every 5 Minutes
		if (!wifi_connected)
		{ // Try to reconnect if disconnected
			// ConnectWifi();
			ConnectFirebase();
		}
		else
		{
			SendWiFiTimestamp(); // Send status
		}
		wifi_last_status_sent = millis();
	}
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

RTC_DATA_ATTR boolean wifi_config_saved = false;
RTC_DATA_ATTR char clocalIP[16];
RTC_DATA_ATTR char cgatewayIP[16];
RTC_DATA_ATTR char csubnetIP[16];
RTC_DATA_ATTR char cdnsIP[16];
RTC_DATA_ATTR uint8_t channel;
RTC_DATA_ATTR uint8_t bssid[6];


const uint8_t* convertCharToUint8Array(const char* chr) {
	static uint8_t octets[4]; // Static array to hold the octets
    
    // Use sscanf to parse the IP address string
    sscanf(chr, "%hhu.%hhu.%hhu.%hhu", &octets[0], &octets[1], &octets[2], &octets[3]);

    return octets; // Return the pointer to the array
}

RTC_DATA_ATTR char tere[10] = {0};
boolean ConnectWifi(const char* ssid, const char* password)
{
	if(strlen(ssid) == 0){
		DBGL("Missing SSID");
		return false;
	}
	WiFi.persistent(false);

	if(wifi_config_saved) {
		DBGL("Taking from saved wifi config");
		IPAddress local = IPAddress(convertCharToUint8Array(clocalIP));
		IPAddress gateway = IPAddress(convertCharToUint8Array(cgatewayIP));
		IPAddress subnet = IPAddress(convertCharToUint8Array(csubnetIP));
		IPAddress dns = IPAddress(convertCharToUint8Array(cdnsIP));

		WiFi.config(local, gateway, subnet, dns);
		// DBGL("Using saved config");
		// DBG("Local: ");DBGL(local.toString());
		// DBG("Gateway: ");DBGL(gateway.toString());
		// DBG("Subent: ");DBGL(subnet.toString());
		// DBG("DNS: ");DBGL(dns.toString());
		WiFi.mode(WIFI_STA);
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
	
	if (WiFi.status() == WL_CONNECTED)
	{
		if(!wifi_config_saved) {
			strcpy(clocalIP, WiFi.localIP().toString().c_str());
			strcpy(cdnsIP, WiFi.dnsIP().toString().c_str());
			strcpy(cgatewayIP, WiFi.gatewayIP().toString().c_str());
			strcpy(csubnetIP, WiFi.subnetMask().toString().c_str());

			/* ---- Might not be needed ---- */
			uint8_t* pbssid = WiFi.BSSID();
			if(pbssid != nullptr){
				memcpy(bssid, pbssid, 6);
			}
			channel = (uint8_t)WiFi.channel();

			for (size_t i = 0; i < 6; i++)
			{
				Serial.print(bssid[i], HEX);
				DBG(" ");
			}
			DBGL("");
			DBG("Channel: "); DBGL(channel);
			/* ------------------------------ */

			DBGL("Saved WiFi config:");
			// DBGL(clocalIP);
			// DBGL(cdnsIP);
			// DBGL(cgatewayIP);
			// DBGL(csubnetIP);

			wifi_config_saved = true;
		}
		wifi_connected = true;
	}
	else
	{
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

	// Firebase.reconnectNetwork(true);
	fbdo.setResponseSize(4096);

	/* Assign the callback function for the long running token generation task */
	config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

	signupOK = true;

	Firebase.begin(&config, &auth);
	// Firebase.reconnectNetwork(true);
}