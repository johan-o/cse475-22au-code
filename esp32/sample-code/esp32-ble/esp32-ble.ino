/*
 * Prints RTC Unix time to serial
 * Transmits unix time over bluetooth
*/
#include <time.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// separate service for data setting
#define DATA_SERVICE_UUID	"4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define DATA_UUID			"beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* dataServer;
BLEService* dataService;
BLECharacteristic *pm10Characteristic;

void setup() {
	Serial.begin(115200);

	BLEDevice::init("ESP32 BLE DATA");
	dataServer = BLEDevice::createServer();
	dataService = dataServer->createService(DATA_SERVICE_UUID);
	pm10Characteristic = dataService->createCharacteristic(
			DATA_UUID,
			BLECharacteristic::PROPERTY_READ |
			BLECharacteristic::PROPERTY_WRITE | 
			BLECharacteristic::PROPERTY_NOTIFY |
			BLECharacteristic::PROPERTY_INDICATE
	);

	pm10Characteristic->setValue("0");

	dataService->start();
	// BLEAdvertising *pAdvertising = dataServer->getAdvertising();  // this still is working for backward compatibility
	BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(DATA_SERVICE_UUID);
	pAdvertising->setScanResponse(true);
	pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
	pAdvertising->setMinPreferred(0x12);
	BLEDevice::startAdvertising();
}

void loop() {
	int seconds = (int) time(NULL);
	std::string pm10CharString;


	Serial.println(seconds);
	pm10Characteristic->setValue(secondsPointer);
	delay(500);
}
