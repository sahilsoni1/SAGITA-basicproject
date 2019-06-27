#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#define ep_Buffer 10
enum  payloadData
{
  sTime = 101,
  button1 = 111
};
class eeprom
{
  public:
    eeprom() {
      Serial.println("eeprom_start");
    }
    ~eeprom() {
      Serial.println("stop");
    }
    friend void destructTest(eeprom* );
    String write_bytes(int position, const char* Data) {
      int i = 0;
      if (position > 0 && strlen(Data) > 1 && strlen(Data) < ep_Buffer ) {
        delayMicroseconds(1);
        for (i = 0; i < ep_Buffer; i++) {
          if (i < strlen(Data)) {
            EEPROM.write((i + position), Data[i]);
          }
          else {
            EEPROM.write((i + position), 0);
            if (i == (ep_Buffer - 1)) {
              EEPROM.commit();
              //            free(Data);
              return "done";
            }
          }
        }
      }
      //   free(Data);
      return "ERROR_003";
    }
    String read_bytes(int position) {
      String readdata = "";
      for (int i = 0; i < ep_Buffer; i++) {
        if ((EEPROM.read(i + position) != 0 ) && (EEPROM.read(i + position) != 255 )) {
          readdata += (char )EEPROM.read(i + position);
        }
        else {
          return readdata;
        }
      }
      return "ERROR_003";
    }
    void epromclear() {
      for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
    }
};
class json: public eeprom
{
  public:
    json() {
      Serial.println("eeprom_start");
    }
    ~json() {
      Serial.println("stop");
    }
    String jsonGenerator(int max, ...) {
      va_list arg_ptr;
      int args = 0;
      DynamicJsonDocument doc(200);
      va_start(arg_ptr, max);
      while (args < max) {
        args = args + 2;
        doc[va_arg(arg_ptr, char*)] = va_arg(arg_ptr, char*);
      }
      String output;
      va_end(arg_ptr);
      serializeJson(doc, output);
      return output;
    }
    void jsonExtrator(int max, const char *data, ...) {
      va_list arg_ptr;
      int args = 0;

      DynamicJsonDocument doc(200);
      DeserializationError error = deserializeJson(doc, (const char*)data);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }
      //   const char* keys[max];
      //   int position[max];
      //const char* sensor = doc["sensor"];
      // long time = doc["time"];
      va_start(arg_ptr, max);
      // Serial.println(sensor);
      //  Serial.println(time);
      while (args < max)
      {
        //  Serial.println(va_arg(arg_ptr, char *));
        const char* abc = doc[va_arg(arg_ptr, char *)];
        // Serial.println(abc);
        int position1 = va_arg(arg_ptr, int);
        write_bytes(position1, abc);
        Serial.println(read_bytes(position1));
        //  Serial.println(va_arg(arg_ptr, int));
        args++;
      }
      va_end(arg_ptr);
    }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  EEPROM.begin(512);
  json js;
  char 
  js.jsonExtrator(2, "{\"sensor\":\"gps\",\"time\":\"135182412\"}", "sensor", sTime, "time", button1);
  js.~eeprom();
  js.~json();
}

void loop() {

}
