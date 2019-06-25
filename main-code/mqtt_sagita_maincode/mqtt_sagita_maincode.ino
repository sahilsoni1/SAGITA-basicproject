#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#define ep_Buffer 10
const char *ssidforhotspot = "SAGITA";
const char *passwordforhotspot = "thereisnospoon";
IPAddress ip(192, 168, 0, 177);
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds
ESP8266WebServer server(80);
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
    String write_bytes(int position, char* Data) {
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
              free(Data);
              return "done";
            }
          }
        }
      }
      free(Data);
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
    }
    void epromclear() {
      for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
    }
};
struct Emp
{
  size_t counter;
};
typedef struct Emp E1;
void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  WiFi.mode(WIFI_AP_STA);
  pinMode(0, INPUT_PULLUP);//flash
  WiFi.softAP(ssidforhotspot, passwordforhotspot);
  server.on("/", handleRoot);
  server.on("/write", handlewrite); //Associate the handler function to the path
  server.on("/read", handleread); //Associate the handler function to the path
  server.begin();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  E1 *ptr;
  ptr = (struct Emp*)malloc(sizeof(struct Emp));
  if (ptr == NULL) {
    Serial.println("no memmory");
  }
  else {
    eeprom EP1;
    ptr->counter = 0;
    while (1) {
      if (EP1.read_bytes(501) != "COMMON") {
        server.handleClient();
        if (millis() - lastConnectionTime > postingInterval) {
          ptr->counter++;
          if (ptr->counter == 5) {
            ptr->counter = 0;
          }
          lastConnectionTime = millis();
        }
      }
      else {
        break;
      }
    }
    String temp = EP1.read_bytes(1);
    String temp1 = EP1.read_bytes(11);
    const char *ssid = temp.c_str();
    const char *pwd = temp1.c_str();
    Serial.println(WiFi.begin(ssid, pwd));
    while (WiFi.status() != WL_CONNECTED) {
      server.handleClient();
      if (millis() - lastConnectionTime > postingInterval) {
        clear_reset();
        server.handleClient();
        Serial.println(".");
        ptr->counter++;
        if (ptr->counter == 10) {
          ptr->counter = 0;
          Serial.println("{4}/9-*jio_error+");
        }
        lastConnectionTime = millis();
      }
    }
    EP1.~eeprom();
  }

}
void loop() {
  clear_reset();
}


void destructTest(eeprom* ep)
{
  delete ep;
  Serial.println("delete");
}
void handleRoot() {
  server.send(200, "text / plain", "You are connected");
}
void handlewrite() {
  int epromaddress, Length;
  static char Data[ep_Buffer];
  String message;
  if (server.arg("address") == "" && server.arg("data") == "" ) {   //Parameter not found
    message = "ERROR_001";
  }
  else {    //Parameter found
    epromaddress = server.arg("address").toInt();
    server.arg("data").toCharArray(Data, ep_Buffer);
    Length = server.arg("data").length();
    if (epromaddress % 10 == 1) {
      eeprom* a;
      message = a->write_bytes(epromaddress, Data);
    }
    else {
      message = "ERROR_002";
    }
  }
  server.send(200, "text / plain", message);        //Returns the HTTP response
}
void handleread() {
  String message;
  int epromaddress;
  if (server.arg("address") == "") {   //Parameter not found
    message = "ERROR_001";
  }
  else {    //Parameter found
    epromaddress = server.arg("address").toInt();
    if (epromaddress % 10 == 1) {
      eeprom* a;
      message = a->read_bytes(epromaddress);
    }
    else {
      message = "ERROR_002";
    }
  }
  server.send(200, "text / plain", message);        //Returns the HTTP response
}
void clear_reset() {
  int data = 0;
ineligible:
  int sensorVal = digitalRead(0);
  if (sensorVal == 0) {
    Serial.println("False");
    Serial.println(data);
    delay(1000);
    data++;
    if (data == 5) {
      eeprom eprom_clear;
      eprom_clear.epromclear();
      ESP.restart();
    }
    goto ineligible;
  }
}
