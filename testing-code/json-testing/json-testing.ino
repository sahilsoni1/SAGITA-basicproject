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
  payloadData p = sTime;
  if (p == 101) {
    Serial.println("yes yes yes!");
  }
  Serial.println("hello");
  Serial.println( jsonGenerator(6, "tem", "30c", "hum", "200", "MOS", "300"));
  // vout(3, "Sat", sTime, "Sun", button1);
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
  char *key[max / 2];
  char *data[max / 2];
  DynamicJsonDocument doc(200);
  va_start(arg_ptr, max);
  while (args < max) {
    //  key[args++] = va_arg(arg_ptr, char*);
    args++;
    //  data[args] = va_arg(arg_ptr, char*);
    doc[va_arg(arg_ptr, char*)] = va_arg(arg_ptr, char*);
    args++;
  }
  String output;
  va_end(arg_ptr);
  serializeJson(doc, output);
  return output;
}
/*
  void jsonExtrator(int max, char *data, ...) {
  va_list arg_ptr;
  int args = 0;
  char key[max];
  int position[max];
  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, (byte*)json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  va_start(arg_ptr, max);
  while (args < max)
  {
    key[args] = va_arg(arg_ptr, char *);
    data[args] = va_arg(arg_ptr, int);
    String data1 = root[days[args]].as<String>();
    write_bytes(position[args], data1);
    Serial.println(position[args++]);

  }
  va_end(arg_ptr);
  }
*/
