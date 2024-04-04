#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

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

void initBleSerial() {


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

void bleSerialTask() {
	static boolean first_ble_connect = true;
	if (deviceConnected)
	{
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