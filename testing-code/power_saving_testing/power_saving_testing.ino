//#include "Maison.h" /* WiFi settings */
#define LED_BUILTIN 2

#define MQTT_PUBTOPIC  “TestPub”
#define MQTT_CLIENT “TestWiFiDS”

IPAddress adr_ip(192, 168, 0, 17);
IPAddress adr_gateway(192, 168, 0, 10);
IPAddress adr_dns(192, 168, 0, 3);

WiFiClient clientWiFi;
PubSubClient clientMQTT(clientWiFi);

void connexion_WiFi(){
WiFi.forceSleepWake();  // Réveil le WiFi
delay(10);
Serial.println(“Connexion WiFi”);

WiFi.persistent( false );

WiFi.mode(WIFI_STA);
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
while(WiFi.status() != WL_CONNECTED){
delay(500);
Serial.print(“.”);
}

Serial.print(“ok : adresse “);
Serial.println(WiFi.localIP());
}

void Connexion_MQTT(){
digitalWrite(LED_BUILTIN, LOW);
Serial.println(“Connexion MQTT”);
while(!clientMQTT.connected()){
if(clientMQTT.connect(MQTT_CLIENT)){
Serial.println(“connecté”);
break;
} else {
Serial.print(“Echec, rc:”);
Serial.println(clientMQTT.state());
delay(1000);  // Test dans 1 seconde
}
}
digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
long debut = millis();  // Calcul du temps de connexion

/*
* Init hardward
*/
Serial.begin(115200); // debug
pinMode(LED_BUILTIN, OUTPUT); // La LED est allumée pendant la recherche de WiFi

/*
* Init WiFi
*/
WiFi.config(adr_ip, adr_gateway, adr_dns);  // Service de base du réseau
connexion_WiFi();

/*
* Envoi du message MQTT
*/
clientMQTT.setServer(BROKER_HOST, BROKER_PORT);
Connexion_MQTT();

long q=millis();
clientMQTT.publish(MQTT_PUBTOPIC, String( q-debut ).c_str());
Serial.println( String( q-debut ).c_str() );

WiFi.disconnect( true );  // Explicitement désactive le WiFi
delay( 1 );
ESP.deepSleep(10e6, WAKE_RF_DISABLED);
}

void loop() {
}


}
void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
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
}
