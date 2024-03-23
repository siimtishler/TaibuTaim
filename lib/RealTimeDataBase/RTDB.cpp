#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
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
	if (Firebase.ready() && signupOK && (millis() - sendDataPrev > 200 || sendDataPrev == 0))
	{
		sendDataPrev = millis();
		return true;
	}
	return false;
}

// Returns JSON format log entry
static FirebaseJson SetLogEntryJSON(String tagUUID, String name, String type, String action)
{
	FirebaseJson fbjson;
	fbjson.add("name", name);
	fbjson.add("UUID", tagUUID);
	fbjson.add("type", type);
	fbjson.add("Event", action);

	return fbjson;
}

// Inits and returns JSON format student
static FirebaseJson SetStudentJSON()
{
	FirebaseJson fbjson;
	struct tm semester_end = {0};				   // Gets semester end date
	unsigned long long semester_end_timestamp = 0; // Converts into timestamp
	fbjson.add("owner", "TBD");
	fbjson.add("type", "student");					  // Default value is student.
	fbjson.add("date_valid", semester_end_timestamp); // From web interface can change added card type

	return fbjson;
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

static bool SetBatteryVoltage(float voltage)
{
	if (IsFireBaseReady())
	{
		String voltage_str = String(voltage);
		if (Firebase.RTDB.setStringAsync(&fbdo, "battery/voltage", voltage_str))
		{
			DBG("Voltage: ");
			DBGL(voltage_str);
			return true;
		}
		else
		{
			DBGL(fbdo.errorReason());
		}
	}
	return false;
}

static bool SetBatteryChgRate(float chg_rate)
{
	if (IsFireBaseReady())
	{
		String chg_rate_str = String(chg_rate);
		if (Firebase.RTDB.setStringAsync(&fbdo, "battery/chgrate", chg_rate_str))
		{
			DBG("Charge Rate: ");
			DBGL(chg_rate_str);
			return true;
		}
		else
		{
			DBGL(fbdo.errorReason());
		}
	}
	return false;
}

static bool SetBatteryPercentage(float percentage)
{
	if (IsFireBaseReady())
	{
		String percentage_str = String(percentage);
		if (Firebase.RTDB.setStringAsync(&fbdo, "battery/percentage", percentage_str))
		{
			DBG("Percentage: ");
			DBGL(percentage_str);
			return true;
		}
		else
		{
			DBGL(fbdo.errorReason());
		}
	}
	return false;
}

void sendBatteryStatus(gauge_measurements_t measurements)
{
	static uint32_t last_send = 0;
	if (millis() - last_send > BATTERY_SEND_TIMEOUT)
	{
		DBGL("Sending batt status");
		delay(200);
		SetBatteryVoltage(measurements.voltage);
		delay(200);
		SetBatteryPercentage(measurements.percentage);
		last_send = millis();
	}
}

static boolean SetLux(uint16_t lux){
	if (IsFireBaseReady())
	{
		String lux_str = String(lux);
		if (Firebase.RTDB.setStringAsync(&fbdo, "light/lux", lux_str))
		{
			DBG("Lux: ");
			DBGL(lux_str);
			return true;
		}
		else
		{
			DBGL(fbdo.errorReason());
		}
	}
	return false;
}

static boolean SetWhite(uint16_t white){
	if (IsFireBaseReady())
	{
		String white_str = String(white);
		if (Firebase.RTDB.setStringAsync(&fbdo, "light/white", white_str))
		{
			DBG("White: ");
			DBGL(white_str);
			return true;
		}
		else
		{
			DBGL(fbdo.errorReason());
		}
	}
	return false;
}

static boolean SetAmbient(uint16_t ambient){
	if (IsFireBaseReady())
	{
		String ambient_str = String(ambient);
		if (Firebase.RTDB.setStringAsync(&fbdo, "light/ambient", ambient_str))
		{
			DBG("Ambient: ");
			DBGL(ambient_str);
			return true;
		}
		else
		{
			DBGL(fbdo.errorReason());
		}
	}
	return false;
}

void sendLightSensorStatus(light_sens_measurements_t measurements)
{
	static uint32_t last_send = 0;
	if (millis() - last_send > BATTERY_SEND_TIMEOUT)
	{
		DBGL("Sending light status");
		delay(200);
		SetLux(measurements.lux);
		delay(200);
		SetWhite(measurements.white);
		delay(200);
		SetAmbient(measurements.ambient);
		delay(200);
		last_send = millis();
	}
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
		delay(100);
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

	if (Firebase.signUp(&config, &auth, auth.user.email, auth.user.password))
	{
		Serial.println("Signup OK");
		signupOK = true;
	}
	else
	{
		Serial.printf("%s\n", config.signer.signupError.message.c_str());
	}
	/* Assign the RTDB URL (required) */

	Firebase.reconnectWiFi(true);
	fbdo.setResponseSize(4096);

	/* Assign the callback function for the long running token generation task */
	config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

	signupOK = true;

	Firebase.begin(&config, &auth);
	Firebase.reconnectWiFi(true);
}