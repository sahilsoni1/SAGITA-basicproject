#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
const char *ssidforhotspot = "SAGITA";
const char *passwordforhotspot = "sahil123";
IPAddress ip(192, 168, 0, 177);
ESP8266WebServer server(80);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  EEPROM.begin(512);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssidforhotspot, passwordforhotspot);
  server.on("/", handleRoot);
  server.on("/write", handlewrite); //Associate the handler function to the path
  server.on("/read", handleread); //Associate the handler function to the path
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

}
void handleRoot() {
  server.send(200, "text / plain", "You are connected");
}
void handlewrite() {

  String message = "";

  if (server.arg("address") == "" && server.arg("data") == "" ) {   //Parameter not found
    message = "address and data Argument not found";
  } else {    //Parameter found
    int epromaddress = server.arg("address").toInt();
    if (epromaddress % 20 == 1) {
      eeprom a;
      message = a.write_bytes(epromaddress, server.arg("data"));
    }
    else {
      message = "your data is not specific";
    }
  }
  server.send(200, "text / plain", message);        //Returns the HTTP response
}
void handleread() {

  String message = "";

  if (server.arg("address") == "") {   //Parameter not found
    message = "address and data Argument not found";
  } else {    //Parameter found
    int epromaddress = server.arg("address").toInt();
    if (epromaddress % 20 == 1) {
      eeprom a;
      message = a.read_bytes(epromaddress);
    }
    else {
      //      message = "your data is not specific";
    }
  }
  server.send(200, "text / plain", message);        //Returns the HTTP response
}
String message = "work fine";

if (server.arg("address") == "" && server.arg("data") == "" ) {   //Parameter not found
  message = "address and data Argument not found";
}
server.send(200, "text / plain", message);        //Returns the HTTP response
}
void handleread() {

  String message = "work fine";

  if (server.arg("address") == "") {   //Parameter not found
    message = "address and data Argument not found";
  }
  server.send(200, "text / plain", message);        //Returns the HTTP response
}
