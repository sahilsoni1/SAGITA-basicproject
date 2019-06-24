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
    char* write_bytes(int position, char* Data, size_t len) {
      if (position > 0 && len > 1 && len < ep_Buffer ) {
        delayMicroseconds(1);
        for (int i = 0; i < ep_Buffer; i++) {
          if (i < len) {
            EEPROM.write((i + position), Data[i]);
          }
          else {
            EEPROM.write((i + position), 0);
            if (i == (i + position - 1)) {
              EEPROM.commit();
              return "done";
            }
          }
        }
      }
      return "ERROR_003";
    }
     char* read_bytes(int position) {
      static char readdata[ep_Buffer];
      for (int i = 0; i < ep_Buffer; i++) {
        if ((EEPROM.read(i + position) != 0
            ) && (EEPROM.read(i + position) != 255 )) {
          readdata[i] = (char )EEPROM.read(i + position);
          delayMicroseconds(1);
        }
        else {
          readdata[i] = NULL;
          return (char *) readdata;
        }
      }
      return "error";
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
  char *ssid;
  char *pwd;
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
  //  server.on("/read", handleread); //Associate the handler function to the path
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
    int count = 0;
    while (1) {
      if (EP1.read_bytes(501) != "COMMON" && EP1.read_bytes(501) != "UNCOMMON") {
        server.handleClient();
        if (millis() - lastConnectionTime > postingInterval) {
          count++;
          if (count == 5) {
            Serial.println("ab");
            ptr->ssid = EP1.read_bytes(1);
            Serial.println(ptr->ssid);
            Serial.println(strlen(ptr->ssid));
            if (strcmp(ptr->ssid,"soni")==0) {
              Serial.println("hello1");
            }
            for (int i = 0; i < 10; i++) {
              Serial.print(i);
              Serial.println(ptr->ssid[i]);
            }
            Serial.println((char*)EP1.read_bytes(501));
            if (EP1.read_bytes(501) != "COMMON") {
              Serial.println("hello");
            }
            count = 0;
          }
          lastConnectionTime = millis();
        }
      }
      else {
        ESP.restart();
        break;
      }
    }
    ptr->ssid = EP1.read_bytes(1);
    Serial.println((char*)ptr->ssid);
    //  E12->write_bytes(10, "sahil123", 6);
    // ptr->pwd = E12->read_bytes(10);
    // destructTest(E12);

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
  char* message;
  if (server.arg("address") == "" && server.arg("data") == "" ) {   //Parameter not found
    message = "ERROR_001";
  }
  else {    //Parameter found
    epromaddress = server.arg("address").toInt();
    server.arg("data").toCharArray(Data, ep_Buffer);
    Length = server.arg("data").length();
    if (epromaddress % 10 == 1) {
      eeprom* a;
      message = a->write_bytes(epromaddress, Data, Length);
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
