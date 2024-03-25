#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include "RTDB.h"
#include "API.h"

#include "timeutils.h"

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// #define DEBUG
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
	while (!(Firebase.ready() && signupOK && (millis() - sendDataPrev > 200 || sendDataPrev == 0)))
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
	pfbjson->add("humidty", String(measurements.humidity));
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
		if (Firebase.RTDB.pushJSONAsync(&fbdo, "battery/", &fbjson))
		{
			String str;
			fbjson.toString(str,true);
			DBGL(str);
			return true;
		}
		DBGL(fbdo.errorReason());
	}
	return false;
}

static bool PushHumidityData(FirebaseJson fbjson)
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.setJSONAsync(&fbdo, "humidity/", &fbjson))
		{
			String str;
			fbjson.toString(str,true);
			DBGL(str);
			return true;
		}
		DBGL(fbdo.errorReason());
	}
	return false;
}

static bool PushLightData(FirebaseJson fbjson)
{
	if (IsFireBaseReady())
	{
		if (Firebase.RTDB.setJSONAsync(&fbdo, "light/", &fbjson))
		{
			String str;
			fbjson.toString(str,true);
			DBGL(str);
			return true;
		}
		DBGL(fbdo.errorReason());
	}
	return false;
}

static bool PushSoilData(uint16_t soilMoisture)
{
	if (IsFireBaseReady())
	{
		String str_soilMoisture = String(soilMoisture);
		if (Firebase.RTDB.setStringAsync(&fbdo, "soil_moisture/", str_soilMoisture.c_str()))
		{
			return true;
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
			ConnectWifi();
			ConnectFirebase();
		}
		else
		{
			SendWiFiTimestamp(); // Send status
		}
		wifi_last_status_sent = millis();
	}
}

// Creates WiFi connection
void ConnectWifi()
{
	static int i = 0;
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	DBGL("Connecting to Wi-Fi");

	unsigned long startMillis = millis();

	while (WiFi.status() != WL_CONNECTED && millis() - startMillis < 5000)
	{
		DBG(".");
		delay(50);
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		wifi_connected = true;
		DBGL();
		DBG("Connected with IP: ");
		DBGL(WiFi.localIP());
		DBGL();
	}
	else
	{
		DBGL("\nWi-Fi connection failed. Timeout reached.");
		wifi_connected = false;
		// Handle the failure or retry if needed
	}
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

	Firebase.reconnectWiFi(true);
	fbdo.setResponseSize(4096);

	/* Assign the callback function for the long running token generation task */
	config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

	signupOK = true;

	Firebase.begin(&config, &auth);
	Firebase.reconnectWiFi(true);
}