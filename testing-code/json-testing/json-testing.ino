enum  payloadData
{
  sTime = 101,
  button1 = 111
};
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
void vout(int max, ...);
void setup() {
  Serial.begin(9600);
  delay(1000);
  //payloadData p = sTime;
  // if (p == 101) {
  //  Serial.println("yes yes yes!");
  // }
  // Serial.println("hello");
  Serial.println( jsonGenerator(6, "tem", "30c", "hum", "200", "MOS", "300"));
  // vout(3, "Sat", sTime, "Sun", button1);
  jsonExtrator(2, "{\"sensor\":\"gps\",\"time\":\"1351824120\"}", "sensor", sTime, "time", button1);
}

void loop() {
  // put your main code here, to run repeatedly:

}
/*void vout(int max, ...)
  {
  va_list arg_ptr;
  int args = 0;
  char *days[max];
  payloadData p[max];
  va_start(arg_ptr, max);
  while (args < max)
  {
    days[args] = va_arg(arg_ptr, char *);
    p[args] = va_arg(arg_ptr, payloadData);
    printf("Day:  %s  \n", days[args]);
    printf("p:  %s  \n", p[args++]);
  }
  va_end(arg_ptr);
  }*/
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

void jsonExtrator(int max, char *data, ...) {
  va_list arg_ptr;
  int args = 0;

  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, (char*)data);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  const char* keys[max];
  int position[max];
  //const char* sensor = doc["sensor"];
 // long time = doc["time"];
  va_start(arg_ptr, max);
 // Serial.println(sensor);
//  Serial.println(time);  
  while (args < max)
  {

  //  Serial.println(va_arg(arg_ptr, char *));
    const char* abc=doc[va_arg(arg_ptr, char *)];
    Serial.println(abc);
    Serial.println(va_arg(arg_ptr, int));
    args++;
  }
  va_end(arg_ptr);
}
