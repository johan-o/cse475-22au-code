// RX of sensor to pin 17
// TX of sensor to pin 16
#include <HardwareSerial.h>
 
#define SECONDS_BETWEEN_READINGS 15
#define NUM_READINGS_CACHED 10

#define DELAY_FAIL 2000

void setup() {
  // our debugging output
  Serial.begin(115200);
 
  // sensor baud rate is 9600
  Serial2.begin(9600);
}
 
struct PMS5003RawData {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct PMS5003RawData rawData;

struct TimeStampedData {
  int t;
  int data;
};
struct TimeStampedData data[NUM_READINGS_CACHED];

void loop() {
  Serial.println("Beginning data read");
  bool readSuccessful = false;

  while (!readSuccessful) {
    readSuccessful = readPMSrawData(&Serial2);
    delay(DELAY_FAIL);
  }

  Serial.println("Successful data read");
  printRawData();
  struct TimeStampedData thisData = calculateAirIndex();
  delay(SECONDS_BETWEEN_READINGS * 1000);
}

// Calculates an air index from sensor readings, currently an average
// Returns a TimeStampedData struct with current time and averaged index
struct TimeStampedData calculateAirIndex() {
  struct TimeStampedData thisData;
  thisData.t  = millis() / 1000;
  thisData.data = (rawData.particles_03um + rawData.particles_05um + rawData.particles_10um + 
      rawData.particles_25um + rawData.particles_50um + rawData.particles_100um);
  
  Serial.print("AVERAGED DATA: ");
  Serial.print((int) thisData.t);
  Serial.print(" ");
  Serial.println((int) thisData.data);
  return thisData;
}

// Prints all parameters from rawData struct
void printRawData() {
  Serial.println();
  Serial.println("---------------------------------------");
  Serial.println("Concentration Units (standard)");
  Serial.print("PM 1.0: "); Serial.print(rawData.pm10_standard);
  Serial.print("\t\tPM 2.5: "); Serial.print(rawData.pm25_standard);
  Serial.print("\t\tPM 10: "); Serial.println(rawData.pm100_standard);
  Serial.println("---------------------------------------");
  Serial.println("Concentration Units (environmental)");
  Serial.print("PM 1.0: "); Serial.print(rawData.pm10_env);
  Serial.print("\t\tPM 2.5: "); Serial.print(rawData.pm25_env);
  Serial.print("\t\tPM 10: "); Serial.println(rawData.pm100_env);
  Serial.println("---------------------------------------");
  Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(rawData.particles_03um);
  Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(rawData.particles_05um);
  Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(rawData.particles_10um);
  Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(rawData.particles_25um);
  Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(rawData.particles_50um);
  Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(rawData.particles_100um);
  Serial.println("---------------------------------------");
}

// Reads PMS data from the stream *s
// Use Serial2 as parameter s
// Returns true only on good data read (non-zero requirement)
boolean readPMSrawData(Stream *s) {
  Serial.print("Reading raw PMS data: ");
  if (! s->available()) {
    Serial.println("Read Failure--Stream *s not available");
    return false;
  }
  
  // wait until 0x42 start byte
  if (s->peek() != 0x42) {
    s->read();
    Serial.println("Read Failure--0x42 start byte not present");
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
 
  // The rawData comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into struct
  memcpy((void *)&rawData, (void *)buffer_u16, 30);
 
  if (sum != rawData.checksum) {
    Serial.println("Read failure--Checksum not matched");
    return false;
  }

  // wait until sensor is initialized
  if (rawData.particles_03um |
      rawData.particles_05um | 
      rawData.particles_10um | 
      rawData.particles_25um | 
      rawData.particles_50um | 
      rawData.particles_100um ) {
        return true;
  }

  Serial.println("Read failure--All zero data");
  return false;
}
