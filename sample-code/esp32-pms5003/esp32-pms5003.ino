#include "PMS.h"
#include <HardwareSerial.h>
#include <Wire.h>
// RX to pin 17
// TX to pin 16
 
PMS pms(Serial2);
PMS::DATA data;

 
String val1;
String val2;
String val3;
 
 
void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600);
 
  Serial.print("Initializing....");
  delay(3000);
}
 
void loop()
{
  if (pms.read(data))
  {
    val1 = data.PM_AE_UG_1_0;
    val2 = data.PM_AE_UG_2_5;
    val3 = data.PM_AE_UG_10_0;
 
    Serial.println("Air Quality Monitor");
 
    Serial.println("PM1.0 :" + val1 + "(ug/m3)");
 
    Serial.println("PM2.5 :" + val2 + "(ug/m3)");
 
    Serial.println("PM10  :" + val3 + "(ug/m3)");
 
    Serial.println("");
 
    delay(2000);
  } else {
//    Serial.println("PMS Read failed");
  }
}
