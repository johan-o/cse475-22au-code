/*
 * Allows the ESP32's RTC to be set by the phone upon pairing
 * Prints RTC Unix time to serial
*/
#include <time.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// separate service for rtc setting
#define RTC_SERVICE_UUID    "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define RTC_UUID            "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define DATA_SERVICE_UUID    "b14c20da-e0a3-47b2-803b-abcbc24b060b"
#define DATA_UUID            "6cd4d98b-297c-43e8-b54d-048ba50d7265"

void setup() {
    Serial.begin(115200);

    BLEDevice::init("ESP32 BLE RTC");
    BLEServer *rtcServer = BLEDevice::createServer();
    BLEService *rtcService = rtcServer->createService(RTC_SERVICE_UUID);
    BLECharacteristic *rtcServerCharacteristic = rtcService->createCharacteristic(
            RTC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    rtcServerCharacteristic->setValue("Hello World says Neil");
    rtcService->start();
    // BLEAdvertising *pAdvertising = rtcServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(RTC_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
	int seconds = getUnixTime();
	Serial.println(seconds);
    delay(500);
}

int getUnixTime() {
	return (int) time(NULL);
}