#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;
#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is conntec to the Arduino digital pin 2
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while (1);
  }
  if (! bme.begin(&Wire)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(seesaw());
  Serial.println(BMET());
  Serial.println(BMEH());
  Serial.println(DH18B20());
  delay (1000);
}
String seesaw()
{
  int capread = ss.touchRead(0);
  String cap = String(capread);
  return cap;
}

String BMET()
{
  String data = String(bme.readTemperature());
  data += " *C,";
  return data;
}
String BMEH()
{
  float data = bme.readHumidity();
  Serial.println("data");
  Serial.println(data);
  String output = String(data);
  output = "%";
  return output;
}
String DH18B20()
{
  sensors.requestTemperatures();
  String soil_temp = String(sensors.getTempCByIndex(0));
  soil_temp += " *C,";
  return soil_temp;
}
