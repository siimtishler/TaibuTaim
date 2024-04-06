// #include <Arduino.h>
// #include "lightsensor.h"
// #include "gauge.h"
// #include "soilsensor.h"
// #include "humiditysensor.h"

// #include "sensorstask.h"

// #include "RTDB.h"

// #include "timeutils.h"

// #define DEBUG
// #include "SerialDebug.h"

// light_sens_measurements_t light_measurements = {0};
// gauge_measurements_t gauge_measurements = {0};

// #include "Wire.h"

// RTC_DATA_ATTR int bootcnt = 0;

// RTC_DATA_ATTR boolean gaugePowerOn = false;
// RTC_DATA_ATTR boolean humiditySensPowerOn = false;
// RTC_DATA_ATTR boolean lightSensPowerOn = false;

// void powerOnDevices() {

// 	if(!gaugePowerOn) {
// 		if(powerOnGauge()){
// 			// DBGL("Powering gauge");
// 			gaugePowerOn = true;
// 		}
// 	}

// 	if(!lightSensPowerOn) {
// 		if(powerOnLightSensor()){
// 			// DBGL("Powering light sensor");
// 			lightSensPowerOn = true;
// 		}
// 	}

// 	if(!humiditySensPowerOn) {
// 		// Have to stop I2C interface for rewiring SDA and SCL for humidity sens
// 		Wire.end();
// 		delay(1);
// 		if(powerOnHumiditySensor()) {
// 			// DBGL("Powering humidity sensor");
// 			humiditySensPowerOn = true;
// 		}
// 	}
// }

// void setup()
// {

// 	Serial.begin(921600);
// 	DBGL("*******/TERE TAIBUTAIM\\*******");
// 	DBGL("");
// 	DBGL("-----------------");
// 	bootcnt++;
// 	Serial.println("Boot number: " + String(bootcnt));

// 	// powerOnDevices();

// 	// initSoilSensor();

// 	// sensorsTask();

// 	// ConnectWifi();
// 	// ConnectFirebase();

// 	// if(initLightSensor()) {
// 	// 	lightSensorPowerSaverEnable(false);
// 	// 	light_measurements = getLightMeasurements();
// 	// 	lightSensorPowerSaverEnable(true);
// 	// }
// 	// send(getLightMeasurements(), "sitt/");

// 	initBleSerial();
// 	DBGL("Sleeping");
// 	DBG("-----------------");
// 	// esp_deep_sleep(SEC_TO_US(10));
// }

// int i = 0;

// void loop()
// {

// }

/*
	Video: https://www.youtube.com/watch?v=oCMOYS71NIU
	Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
	Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "WiFi.h"
#include "RTDB.h"

#include <Arduino.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

void getAllWiFiSSIDs() {
	uint8_t num = WiFi.scanNetworks();
	if(num == 0) {
		Serial.println("no networks");
	}
	for (int i = 0; i < num; i++)
	{
		// Serial.println(WiFi.SSID(i));
		std::string ssid = ("<SSID>"+WiFi.SSID(i)).c_str();
		pTxCharacteristic->setValue(ssid);
		pTxCharacteristic->notify();
	}
	std::string ssid = "</SSID_END>";
	pTxCharacteristic->setValue(ssid);
	pTxCharacteristic->notify();
}

#define SERVICE_UUID "7f9b95e0-3f72-4f75-92d2-36c70cd39670" // UART service UUID
#define CHARACTERISTIC_UUID_RX "7f9b95e1-3f72-4f75-92d2-36c70cd39670"
#define CHARACTERISTIC_UUID_TX "7f9b95e2-3f72-4f75-92d2-36c70cd39670"

class MyServerCallbacks : public BLEServerCallbacks
{
	void onConnect(BLEServer *pServer)
	{
		deviceConnected = true;
	};

	void onDisconnect(BLEServer *pServer)
	{
		deviceConnected = false;
	}
};

class MyCallbacks : public BLECharacteristicCallbacks
{
	void onWrite(BLECharacteristic *pCharacteristic)
	{
		std::string rxValue = pCharacteristic->getValue();

		if (rxValue.length() > 0)
		{
			Serial.println("*********");
			Serial.println("Received Value: ");

			uint8_t ssid_len = rxValue[0];
			uint8_t pass_len = rxValue[ssid_len + 1];

			String receivedData = String(rxValue.c_str());
			// Extract SSID and password
			String ssid = String(receivedData).substring(1, ssid_len + 1);
			String password = String(receivedData).substring(ssid_len + 2, ssid_len + pass_len + 2);

			Serial.println(ssid);
			Serial.println(password);

			std::string ssid_str = ssid.c_str();

			std::string connection = "Got WiFi name and password";
			pTxCharacteristic->setValue(connection);
			pTxCharacteristic->notify();

			if(ConnectWifi(ssid.c_str(), password.c_str())) {
				connection = "Success";
				pTxCharacteristic->setValue(connection);
				pTxCharacteristic->notify();
				Serial.println("Success");
			}
			else {
				connection = "Failed";
				pTxCharacteristic->setValue(connection);
				pTxCharacteristic->notify();
				Serial.println("Failed");
			}

			Serial.println();
			Serial.println("*********");
		}
	}
};

void setup()
{
	Serial.begin(921600);

	// Create the BLE Device
	BLEDevice::init("TaibuTaim");

	// Create the BLE Server
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	pTxCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID_TX,
		BLECharacteristic::PROPERTY_NOTIFY | 
		BLECharacteristic::PROPERTY_READ);

	pTxCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID_RX,
		BLECharacteristic::PROPERTY_WRITE);

	pRxCharacteristic->setCallbacks(new MyCallbacks());

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Waiting a client connection to notify...");
}



void loop()
{
	static boolean first_ble_connect = true;
	if (deviceConnected)
	{
		// pTxCharacteristic->setValue(&txValue, 1);
		// pTxCharacteristic->notify();
		// txValue++;
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}

	// disconnecting
	if (!deviceConnected && oldDeviceConnected)
	{
		delay(500);					 // give the bluetooth stack the chance to get things ready
		pServer->startAdvertising(); // restart advertising
		Serial.println("start advertising");
		oldDeviceConnected = deviceConnected;
	}
	// connecting
	if (deviceConnected && !oldDeviceConnected)
	{
		if(first_ble_connect){
			getAllWiFiSSIDs();
			first_ble_connect = false;
		}
		Serial.println("Connected");
		oldDeviceConnected = deviceConnected;
	}
}