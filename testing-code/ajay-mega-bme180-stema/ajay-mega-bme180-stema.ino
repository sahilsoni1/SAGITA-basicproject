#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define BME_SCK 52  
#define BME_MISO 50
#define BME_MOSI 51
#define BME_CS 53
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme(BME_CS); 

const int rs = 25, en = 24, d4 = 41, d5 = 40, d6 = 39, d7 = 38;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    //while(1);
  } 
 bool status;
 status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        //while (1);
    }

 lcd.begin(20,4);

}
    
void loop() {
  Serial.println(seesaw());
  lcd.setCursor(0,0);
  lcd.print(seesaw());
  lcd.setCursor(0,1);
  Serial.println(BME_temp());
  lcd.print(BME_temp());
  lcd.setCursor(0,2);
  Serial.println(BME_humidity());
  lcd.print(BME_humidity());
  lcd.setCursor(0,3);
  Serial.println(DH18B20());
  lcd.print(DH18B20());
  delay(1000);
  lcd.clear();
  
  
}
String seesaw()
{
  uint16_t capread = ss.touchRead(0);
  String cap ="Capacitive: ";
  cap +=capread;
  return cap;
}
String BME_temp()
{
    String data ="";
    data = "Temperature: ";
    data+=bme.readTemperature() ;
    return data;
}
String BME_humidity()
{
  String data ="Humidity: ";
  data += bme.readHumidity();
  return data;
}
String DH18B20()
{
  sensors.requestTemperatures(); 
  String soil_temp ="Soil Temp :";
  soil_temp += sensors.getTempCByIndex(0); 
  return soil_temp;
 }
 
