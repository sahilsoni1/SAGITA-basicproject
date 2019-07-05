#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define ep_Buffer 10
#define mqtt_buffer 200
#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;
#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
#include <OneWire.h>
#include <DallasTemperature.h>
////////static configuration
IPAddress adr_ip(192, 168, 0, 17);
IPAddress adr_gateway(192, 168, 0, 10);
IPAddress adr_dns(192, 168, 0, 3);
////temp variable
long lastMsg = 0;
char msg[50];
int value = 0;

// Data wire is conntec to the Arduino digital pin 2
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

enum  payloadData
{
  WIFISSID = 1,
  WIFIPWD = 11,
  CLIENTID = 21,
  sTime = 101,
  button1 = 111,
  VERFICATION = 501
};
const char *ssidforhotspot = "SAGITA";
const char *passwordforhotspot = "thereisnospoon";
const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress ip(192, 168, 0, 177);
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
class eeprom
{
  public:
    eeprom() {
      //  Serial.println("eeprom_start");
    }
    ~eeprom() {
      //  Serial.println("stop");
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
              //              free(Data);
              return "done";
            }
          }
        }
      }
      //      free(Data);
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
      //    Serial.println("eeprom_start");
    }
    ~json() {
      //   Serial.println("stop");
    }
    String jsonGenerator(int max, ...) {
      va_list arg_ptr;
      int args = 0;
      DynamicJsonDocument doc(mqtt_buffer);
      va_start(arg_ptr, max);
      while (args < max) {
        args++;
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

      DynamicJsonDocument doc(mqtt_buffer);
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
        if (strlen(abc) >= ep_Buffer) {
          Serial.println("json key data  is greater than eeprom buffer");
          return;
        }
        write_bytes(position1, abc);
        Serial.println(read_bytes(position1));
        //  Serial.println(va_arg(arg_ptr, int));
        args++;
      }
      va_end(arg_ptr);
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
  delay(1000);
  //  WiFi.forceSleepWake();  // Réveil le WiFi
  //  delay(10);
  //  Serial.println("Connexion WiFi");

  //  WiFi.persistent( false );
  WiFi.mode(WIFI_AP_STA);
  pinMode(0, INPUT_PULLUP);//flash
  //enum  payloadData PD;
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
      if (EP1.read_bytes(VERFICATION) != "COMMON") {
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
    String temp = EP1.read_bytes(WIFISSID);
    String temp1 = EP1.read_bytes(WIFIPWD);
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
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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
  clear_reset();
  if (!client.connected()) {
    reconnect();

  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    //  snprintf (msg, 50, "hello world #%ld", value);
    //Serial.print("Publish message: ");
    //  Serial.println(msg);

    json jsg;


    char outputMsg[mqtt_buffer] ;
    const char* dh18b20 = DH18B20().c_str();
    const char* bmeh = BMEH().c_str();
    const char* seeSaw = seesaw().c_str();
    const char* bmet = BMET().c_str();
    jsg.jsonGenerator(4, "STem", DH18B20().c_str(), "Hum", BMEH().c_str(), "Mos", seesaw().c_str(), "ATem", BMET().c_str()).toCharArray(outputMsg, mqtt_buffer);
    client.publish("SAGITA", outputMsg);
    Serial.println((char*)outputMsg);
    jsg.~json();
    ESP.deepSleep(30e6, WAKE_RF_DEFAULT);
  }
  delay(1000);
  clear_reset();
}






/*void destructTest(eeprom* ep)
  {
  delete ep;
  Serial.println("delete");
  }*/
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
    if (epromaddress % ep_Buffer == 1) {
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
    if (epromaddress % ep_Buffer == 1) {
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
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int count = 0;
  int stop_serial = 0;
  static char a[100];
  for (int i = 0; i < length; i++) {
    a[i] = (char)payload[i];
    if (a[i] == '{') {
      count++;
      stop_serial = 1;
    }
    else if (a[i] == '}') {
      count--;
    }
    if (stop_serial == 1) {
      Serial.print(a[i]);
      if (count == 0 ) {
        a[i + 1] = '\0';
        Serial.println(a[i + 1]);
        break;
      }
    }
  }
  const char* ab = a;
  Serial.print((char*)ab);
  // json js;
  //   js.jsonExtrator(2, ab, "sensor", sTime, "time", button1);
  if (strcmp(topic, "SAGITA_I") == 0) {
    json js;
    js.jsonExtrator(2, ab, "sensor", sTime, "time", button1);
    //js.~json();
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "sahilsoni19970";
    /// Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("SAGITA", "hello world");
      json jsg;
      char outputMsg[mqtt_buffer] ;
      jsg.jsonGenerator(4, "STem", DH18B20().c_str(), "Hum", BMEH().c_str(), "Mos", seesaw().c_str(), "ATem", BMET().c_str()).toCharArray(outputMsg, mqtt_buffer);
      client.publish("SAGITA", outputMsg);
      Serial.println((char*)outputMsg);
      jsg.~json();
      delay(1000);
      ESP.deepSleep(30e6, WAKE_RF_DEFAULT);
      // ... and resubscribe
      client.subscribe("SAGITA_I");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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
  data += " *c";
  return data;
}
String BMEH()
{
  float data = bme.readHumidity();
  String output = String(data);
  output += "%";
  return output;
}
String DH18B20()
{
  sensors.requestTemperatures();
  String soil_temp = String(sensors.getTempCByIndex(0));
  soil_temp += " *C";
  return soil_temp;
}
