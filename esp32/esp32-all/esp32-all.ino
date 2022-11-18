// SET pin
// RX of sensor to pin 17
// TX of sensor to pin 16
#include <HardwareSerial.h>

#include <time.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SECONDS_BETWEEN_READINGS 10
#define NUM_READINGS_CACHED 10

// milliseconds delay between tries to read sensor data
#define DELAY_FAIL 500

// Raw Data from PMS5003 Sensor
struct PMS5003RawData {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct PMS5003RawData data;

// Structure to store data with unix time stamp
struct TimeStampedData {
  uint32_t time;
  uint16_t pm10;
  uint16_t pm25;
  uint16_t pm100;
};

// Bluetooth UUIDs
// Phone needs to set time in the RTC_CHARACTERISTIC to unsigned 32 bit int unix time (since Jan 1 1970)
#define RTC_SERVICE_UUID          "e0894506-fda6-43d4-b1a4-6813e608b549"
#define RTC_CHARACTERISTIC_UUID   "28348d85-8d9a-4733-bcb5-f45ea46851b4"

// Bit Layout:
//  0:3 time
//  4:5 pm1.0
//  6:7 pm2.5
//  8:9 pm10
#define DATA_SERVICE_UUID         "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define DATA_CHARACTERISTIC_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* bleServer;
BLECharacteristic* bleRTCCharacteristic;
BLECharacteristic* bleDataCharacteristic;

// BLE Callback to allow clock to be set immediately upon a write
class MyRTCCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    Serial.print("Setting Clock: ");
    std::string bleRTCValue = pCharacteristic->getValue();

    if (bleRTCValue.size() == 4) {
      uint32_t time = 0;
      for (int i = 0; i < 4; i++) {
        Serial.print((unsigned char) bleRTCValue[i], HEX);
        Serial.print(",");
        Serial.print((uint32_t) bleRTCValue[i] << (8 * i));
        Serial.print(" ");
        time += ((uint32_t) bleRTCValue[i]) << (8 * i);
      }

      struct timeval tv;
      tv.tv_sec = time;
      tv.tv_usec = 0;
      settimeofday(&tv, NULL);

      Serial.print(time);
    }
    Serial.println();
  }
};

void setup() {
  // debugging output
  Serial.begin(115200);
 
  // sensor baud rate is 9600
  Serial2.begin(9600);

  // set reset pin high
  digitalWrite(13, HIGH);

  // Bluetooth
  BLEDevice::init("ESP32 PMS5003");

  bleServer = BLEDevice::createServer();
  
  BLEService *bleRTCService = bleServer->createService(RTC_SERVICE_UUID);
  bleRTCCharacteristic = bleRTCService->createCharacteristic(
                    RTC_CHARACTERISTIC_UUID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
                  );
  bleRTCCharacteristic->setCallbacks(new MyRTCCallbacks());

  BLEService *bleDataService = bleServer->createService(DATA_SERVICE_UUID);
  bleDataCharacteristic = bleDataService->createCharacteristic(
                    DATA_CHARACTERISTIC_UUID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
                  );

  // Start the service and advertising
  bleRTCService->start();
  bleDataService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(RTC_SERVICE_UUID);
  pAdvertising->addServiceUUID(DATA_SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
}

void loop() {
  // setting time
  if ((uint32_t) time(NULL) < 86400) {
    Serial.println("Clock not set, awaiting BLE write");
    delay(500);
  } else { // reading sensor data
    Serial.println("Beginning data read");
    bool readSuccessful = readPMSRawData(&Serial2);
  
    while (!readSuccessful) {
      digitalWrite(13, LOW);
      delay(100);
      digitalWrite(13, HIGH);
      readSuccessful = readPMSRawData(&Serial2);
      delay(DELAY_FAIL - 100);
    }
    printRawData();
  
    Serial.println("Successful data read");
    // printRawData();
    struct TimeStampedData thisData = createDataStruct();
    transmitDataStruct(thisData);
    
    delay(SECONDS_BETWEEN_READINGS * 1000);
  }
}

// Calculates an air index from sensor readings, currently an average
// Returns a TimeStampedData struct with current time and averaged index
struct TimeStampedData createDataStruct() {
  struct TimeStampedData thisData;
  time_t seconds = time(NULL);
  thisData.time  = seconds;
  thisData.pm10 = data.particles_03um + data.particles_05um;
  thisData.pm25 = data.particles_10um + data.particles_25um;
  thisData.pm100 = data.particles_50um + data.particles_100um;
  
  Serial.print("DATA: t=");
  Serial.print(thisData.time, HEX); 
  Serial.print(" pm1.0=");
  Serial.print(thisData.pm10);
  Serial.print(" pm2.5=");
  Serial.print(thisData.pm25);
  Serial.print(" pm10.0=");
  Serial.println(thisData.pm100);
  return thisData;
}

void transmitDataStruct(struct TimeStampedData data) {
  Serial.println("Setting bleDataCharacteristic");
  std::string bleData = "0123456789";

  // Time (Bits 0:3)
  for (int i = 0; i < 4; i++) {
    bleData[i] = data.time >> (8 * i);
  }

  // PM1.0 and below
  bleData[4] = data.pm10;
  bleData[5] = data.pm10 >> 8;

  // PM2.5
  bleData[6] = data.pm25;
  bleData[7] = data.pm25 >> 8;
  
  // PM10.0
  bleData[8] = data.pm100;
  bleData[9] = data.pm100 >> 8;

  Serial.print("String Value: ");
  for(int i = 9; i >= 0; i--) {
    Serial.print((unsigned char) bleData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  bleDataCharacteristic->setValue(bleData);
  bleDataCharacteristic->notify();
}

// Prints all parameters from data struct
void printRawData() {
  Serial.println();
  Serial.println("---------------------------------------");
  Serial.println("Concentration Units (standard)");
  Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
  Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
  Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
  Serial.println("---------------------------------------");
  Serial.println("Concentration Units (environmental)");
  Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
  Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
  Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
  Serial.println("---------------------------------------");
  Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
  Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
  Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
  Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
  Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
  Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
  Serial.println("---------------------------------------");
}

// Reads PMS data from the stream *s
// Use Serial2 as parameter s
// Returns true only on good data read (non-zero requirement)
boolean readPMSRawData(Stream *s) {
  Serial.print("Reading raw PMS data: ");
  if (! s->available()) {
    Serial.println("Read Failure--Stream *s not available");
    return false;
  }
  
  // wait until 0x42 start byte
  if (s->peek() != 0x42) {
    s->read();
    Serial.print("Read Failure--0x42 start byte not present. Actual Value == ");
    Serial.println(s->peek(), HEX);
    return false;
  }
 
  // wait until all 32 bytes available
  if (s->available() < 32) {
    Serial.println("Read Failure--Less than 32 bytes available");
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
 
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into struct
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) {
    Serial.println("Read failure--Checksum not matched");
    return false;
  }

  // wait until sensor is initialized
  if (data.particles_03um |
      data.particles_05um | 
      data.particles_10um | 
      data.particles_25um | 
      data.particles_50um | 
      data.particles_100um ) {
        return true;
  }

  Serial.println("Read failure--All zero data");
  return false;
}
