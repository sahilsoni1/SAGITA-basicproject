#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FirebaseArduino.h>
#include <ESP8266Ping.h>
#include <WifiLocation.h>
#include <WiFiClientSecure.h>
const char *ssidforhotspot = "RR_Node(Default)";
const char *passwordforhotspot = "rr@123456";
char* ssid  ;//   = "RR_WIFI";
char* password ;//= "rr@123456";
String controller_id ;
const char* FIREBASE_HOST = "led-screen-cc6d4.firebaseio.com";
const char* FIREBASE_AUTH = "uklbiSVAwrHChKHwsX00R78wbwPecNs6Ww0TdoEh";
ESP8266WebServer server(80);
const char* remote_host = "www.google.com";
//const char* remote_host = "www.google.com";
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds
unsigned long lastConnectionTime2 = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval2 = 10L * 1000L; // delay between updates, in milliseconds
String datetime_format(String);
class eeprom
{
  public:
    eeprom() {
      //   Serial.println("start");
    }
    ~eeprom() {
      //   Serial.println("stop");
    }
    String write_bytes(int int_address, String data) {
      //Serial.println(data);
      char ab[20];

      data.toCharArray(ab, 20);
      int j = 0;
      for (int i = int_address; i < (int_address + 20); i++) {
        if (j < data.length()) {
          delayMicroseconds(1);
          //  Serial.print(i);
          //  Serial.println(ab[j]);
          EEPROM.write((i), ab[j]);
          j++;
        }
        else {
          EEPROM.write((i), 0);
          if (i == (int_address + 20 - 1)) {
            EEPROM.commit();
            return "done";
          }
        }
      }
    }
    String read_bytes(int int_address) {
      String readdata = "";
      //Serial.print("address");
      // Serial.print(int_address);
      for (int i = int_address; i < (int_address + 20); i++) {
        if ((EEPROM.read(i) != 0 ) && (EEPROM.read(i) != 255 )) {
          readdata += (char )EEPROM.read(i);
        }
        else {
          //   Serial.print(password);
          return readdata;
          break;
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
//char *uploaddata[8];
//char **uploaddata = new char*[10];
/*
  1.controller_id//41 to 60
  2.battery_status = "";
  3.battery_level = "";
  4.phoneNumber//61 to 80;
  5.signalStrength = "";
  6.deviceId //81 to 100
  7.rtc time
  8.apn
  // 7.ac
  //json_extrator(3, "data", 3, "senson", 2, "time", 1);
*/
class jio: public eeprom
{
  public:
    static int httpPort;
    static char* host;
    String data = "";
  public:
    jio() {
      // Serial.println("start");
    }
    ~jio() {
      // Serial.println("stop");
    }
    friend void destructTest(jio* );
    void jiourl(String url) {
      data = "";
      WiFiClient client1;
      if (!client1.connect(host, httpPort)) {
        Serial.println("connection failed");
        return ;
      }
      client1.print(String("GET ") + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (client1.available() == 0) {
        if (millis() - timeout > 5000) {
          client1.stop();
          return ;
        }
      }
      int count = 0;
      while (client1.available()) {
        char line = client1.read();
        if ((line == '{') || (count == 1)) {
          count = 1;
          data += line;
          if (line == '}') {
            client1.stop();
            return;
            break;
          }
        }
        delayMicroseconds(500);
      }
      client1.stop();
    }
    /*extract all json data Maximum size is 7*/
    void jsonExtrator(int max, ...) {
      va_list arg_ptr;
      int args = 0;
      char *days[7];
      int position[7];
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(data);
      if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
      }
      va_start(arg_ptr, max);
      while (args < max)
      {
        days[args] = va_arg(arg_ptr, char *);
        position[args] = va_arg(arg_ptr, int);
        if (root[String(days[args])].as<String>().length() > 1) {
          // const char* abc = root[String(days[args])];
          // uploaddata[position[args]] = const_cast<char *>(abc);
          String data1 = root[days[args]].as<String>();
          if (position[args] == 181) {
            data1 = datetime_format(data1);
          }
          write_bytes(position[args], data1);
          // Serial.println(uploaddata[position[args]]);
          Serial.println(position[args++]);
        }
      }
      va_end(arg_ptr);
    }
    void jsonGenerator() {
      StaticJsonBuffer<300> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      root["relay"] = read_bytes(201);
      root["acStatus"] = digitalRead(16);
      root["batteryStatus"] = read_bytes(161);
      root["batteryLevel"] = read_bytes(141);
      root["Signals"] = read_bytes(101);
      root["simId"] = read_bytes(81);
      root["deviceId"] = read_bytes(61);
      root["controllerName"] = read_bytes(41);
      root["rtcTime"] = read_bytes(181);
      root["APN"] = read_bytes(121);
      String output;
      root.printTo(output);
      Serial.println(output);
      Firebase.setString(controller_id + "/data", output);
    }
    void functionManager(int activityType) {
      switch (activityType) {
        case 1:
          //basic_detail
          this->jiourl("/lte_ajax.cgi");
          this->jsonExtrator(2, "rsrq", 101, "apn", 121);
          break;
        case 2:
          //wifiuser
          // this->jiourl("/lan_ajax.cgi");
          // this->json_extrator(3, "data", 3, "senson", 2, "time", 1);
          break;
        case 3:
          //wificpudetail
          // this->jiourl("/performance_ajax.cgi");
          // this->json_extrator(3, "data", 3, "senson", 2, "time", 1);
          break;
        case 4:
          //ipdetail()
          // this->jiourl("/wan_info_ajax.cgi");
          //  this->json_extrator(3, "data", 3, "senson", 2, "time", 1);
          break;
        case 5:
          //device-detail()
          this->jiourl("/Device_info_ajax.cgi");
          this->jsonExtrator(3, "batterylevel", 141, "batterystatus", 161, "curr_time", 181);
          break;
        default:
          break;
      }
    }
};
int jio::httpPort = 80;//jio class variable
char* jio::host = "jiofi.local.html";//jio class variable
const char* googleApiKey = "AIzaSyBoGQzHp8-F_i-Ug15cT35qQ8g6fmOV_XE";
void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  pinMode(0, INPUT_PULLUP);//flash
  pinMode(16, INPUT_PULLUP);//ac
  pinMode(5, OUTPUT);//relay
  pinMode(14, OUTPUT);//indication
  pinMode(12, OUTPUT);//jio recharge checking
  pinMode(4, OUTPUT);//jio start stop
  eeprom a;
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssidforhotspot, passwordforhotspot);
  digitalWrite(14, 1);
  digitalWrite(12, 0);
  digitalWrite(4, 1);
  if (a.read_bytes(201) == "1") {
    digitalWrite(5, 0);
  }
  else {
    digitalWrite(5, 1);
  }

  if (a.read_bytes(501) != "1" && a.read_bytes(501) != "2") {
    server.on("/", handleRoot);
    server.on("/write", handlewrite); //Associate the handler function to the path
    server.on("/read", handleread); //Associate the handler function to the path
    server.begin();
    int count = 0;
    while (1) {
      if (a.read_bytes(501) != "1" && a.read_bytes(501) != "2") {
        server.handleClient();
        if (millis() - lastConnectionTime > postingInterval) {
          count++;
          if (count == 5) {
            Serial.println("ab");
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

  }
  char ssid1[20];
  char password1[20];
  a.read_bytes(1).toCharArray(ssid1, 20);
  Serial.println(ssid1);
  ssid = ssid1;
  Serial.println(ssid);
  a.read_bytes(21).toCharArray(password1, 20);
  Serial.println(password1);
  password = password1;
  Serial.println(password);


  WiFi.begin(ssid, password);
  int node_count = 0;
  jionetVerfing();
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  controller_id = "ledScreen_list/";
  controller_id += a.read_bytes(41);
  Firebase.setString(controller_id + "/Log/" + logfileName(a.read_bytes(181)) , loggenegrate("node-restart"));
}
int milliscount = 1;
int notPing = 0;

void loop() {


  if (millis() - lastConnectionTime > postingInterval) {
    Serial.println("firebaseerrorstop");
    battery_charger();
    clear_reset();
    milliscount++;
    if (milliscount == 5) {
      jio *ptr = new jio;
      ptr->functionManager(5);
      destructTest(ptr);
    }
    else if (milliscount == 10) {
      jio *ptr1 = new jio;
      ptr1->functionManager(1);
      destructTest(ptr1);
      milliscount = 0;
    }
    else if (milliscount > 10) {
      milliscount = 0;
    }

    eeprom relay;
    String relay_data = Firebase.getString(controller_id + "/relay");
    jionetVerfing();
    Serial.println("checkrelay");
    Serial.println(relay_data);
    relay_data.replace("\"", "");
    Serial.println(relay_data);
    if (relay_data.length() > 0) {
      if (relay_data.toInt() == 1) {
        digitalWrite(5, 0);
        if (relay_data != relay.read_bytes(201))
          Firebase.setString(controller_id + "/Log/" + logfileName(relay.read_bytes(181)) , loggenegrate("screen-on"));

        relay.write_bytes(201, "1");


      }
      else if (relay_data.toInt() == 0) {
        digitalWrite(5, 1);
        if (relay_data != relay.read_bytes(201))
          Firebase.setString(controller_id + "/Log/" + logfileName(relay.read_bytes(181)) , loggenegrate("screen-off"));

        relay.write_bytes(201, "0");
      }
    }
    jio *ptr2 = new jio;
    ptr2->jsonGenerator();

    destructTest(ptr2);
    if (milliscount % 2 == 0) {
      digitalWrite(14, 0);
    }
    else {
      digitalWrite(14, 1);
    }
    lastConnectionTime = millis();
  }

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
void destructTest(jio* ptr)
{
  delete ptr;
  Serial.println("delete");
  //    cout << "Object Destroyed\n";
}
void battery_charger() {
  eeprom bData;
  String batterylevel = bData.read_bytes(141) ;
  batterylevel.replace(" ", "");
  batterylevel.replace("%", "");
  //   Serial.println(batterylevel);
  if (batterylevel.toInt() < 50) {
    digitalWrite(12, 0);
    Serial.println("1");
  }
  else if (batterylevel.toInt() > 95) {

    digitalWrite(12, 1);
    //   Serial.println("2");
  }
  else {
    Serial.println("3");
  }
}
void jio_reset() {
  int count = 0;
  while (WiFi.status() != WL_CONNECTED || notPing == 1) {
    count++;
    battery_charger();
    clear_reset();
    digitalWrite(14, 1);
    // Serial.println("internet notpresent");
    if (count == 5) {
      Serial.println("{4}/9-*jio_error+");
      digitalWrite(4, 1);
    }
    if (count == 10) {
      Serial.println("{4}/9-*jio_error+");
      digitalWrite(4, 0);
    }
    if (count == 13) {
      Serial.println("{4}/9-*jio_error+");
      digitalWrite(4, 1);
    }
    if (count == 23) {
      Serial.println("{4}/9-*jio_error+");
      digitalWrite(4, 0);
    }
    if (count == 70) {
      break;
      Serial.println("{4}/9-*jio_error+");
      count = 0;
    }
    lastConnectionTime = millis();
    Serial.println("back1");
    if (WiFi.status() != WL_CONNECTED || milliscount > 13) {

    }
    else {
      break;
      notPing = 0;
    }
    delay(1000);
  }
}
int wifiVerfiedCount = 0;
int pingVerfiedCount = 0;
int noNetVerfiedCount = 0;
void jionetVerfing() {
  while (1) {
    delay(1000);
    Serial.println("while loop working");
    server.handleClient();
    clear_reset();
    digitalWrite(14, 1);
    if (WiFi.status() != WL_CONNECTED) {
      noNetVerfiedCount = 0;
      Serial.println(wifiVerfiedCount);
      wifiVerfiedCount++;
      if (wifiVerfiedCount > 9) {
        if (wifiVerfiedCount > 9 && wifiVerfiedCount < 14)
          digitalWrite(4, 0);
        else if (wifiVerfiedCount > 14 && wifiVerfiedCount < 20)
          digitalWrite(4, 1);
        else if (wifiVerfiedCount > 100 )
          wifiVerfiedCount = 0;
      }
    }
    else {
      wifiVerfiedCount = 0;
      if (Firebase.failed()) {
        if (Ping.ping(remote_host)) {
          int avg_time_ms = Ping.averageTime();
          Serial.println(avg_time_ms);
          Serial.println("{4}/10-*node_error+");
          ESP.restart();
        }
        else {
          noNetVerfiedCount++;
          if (noNetVerfiedCount > 1 && noNetVerfiedCount < 7)
            digitalWrite(4, 0);
          else if (noNetVerfiedCount > 7 && noNetVerfiedCount < 12)
            digitalWrite(4, 1);
          else if (noNetVerfiedCount > 100 )
            wifiVerfiedCount = 0;
        }
      }
      else {
        break;
      }
    }
  }
}
String logdata(String data, String Time) {
  return "\"" + data + "," + Time + "\"";
}
int Month_abv(String data) {
  if (data == "Jan") {
    return 1;
  }
  else if (data == "Feb") {
    return 2;
  }
  else if (data == "Mar") {
    return 3;
  }
  else if (data == "Apr") {
    return 4;
  }
  else if (data == "May") {
    return 5;
  }
  else if (data == "June") {
    return 6;
  }
  else if (data == "July") {
    return 7;
  }
  else if (data == "Aug") {
    return 8;
  }
  else if (data == "Sept") {
    return 9;
  }
  else if (data == "Oct") {
    return 10;
  }
  else if (data == "Nov") {
    return 11;
  }
  else if (data == "Dec") {
    return 12;
  }
}
int Weekday(String data) {
  if (data == "Mon") {
    return 2;
  } else if (data == "Tue") {
    return 3;
  }  else if (data == "Wed") {
    return 5;
  } else if (data == "Thu") {
    return 4;
  } else if (data == "Fri") {
    return 6;
  } else if (data == "Sat") {
    return 7;
  } else if (data == "Sun") {
    return 1;
  }
}
String datetime_format(String datetime) {
  int sub = 0;
  sub = datetime.indexOf(" ");
  String week = datetime.substring(0, sub);
  week = String(Weekday(week));
  datetime = datetime.substring(sub + 1);
  sub = 0;
  sub = datetime.indexOf(" ");
  String Date = datetime.substring(0, sub);
  datetime = datetime.substring(sub + 1);
  //Serial.println(Date);

  sub = 0;
  sub = datetime.indexOf(" ");
  String Month = datetime.substring(0, sub);
  Month = String(Month_abv(Month));
  datetime = datetime.substring(sub + 1);
  //Serial.println(Month);

  sub = 0;
  sub = datetime.indexOf(" ");
  String Year = datetime.substring(0, sub);
  datetime = datetime.substring(sub + 1);
  //Serial.println(Year);

  sub = datetime.indexOf(":");
  String Hour = datetime.substring(0, sub);
  datetime = datetime.substring(sub + 1);
  //Serial.println(Hour);

  sub = datetime.indexOf(":");
  String Mintue = datetime.substring(0, sub);
  datetime = datetime.substring(sub + 1);
  //Serial.println(Mintue);
  String Second = datetime;
  //Serial.println(Second);

  //Serial.println(datetime);
  if (week.length() > 0 && Date.length() != 0 && Month.length() != 0 && Year.length() != 0 && Hour.length() != 0 && Mintue.length() != 0 && Second.length() != 0) {
    if (Month.length() != 2) {
      Month = "0" + Month;
    }
    if (Date.length() != 2) {
      Date = "0" + Date;
    }
    if (Hour.length() != 2) {
      Hour = "0" + Hour;
    }
    if (Mintue.length() != 2) {
      Mintue = "0" + Mintue;
    }
    if (Second.length() != 2) {
      Second = "0" + Second;
    }
    String live_time = Year + "-" + Month + "-" + Date + "!" + Hour + ":" + Mintue + ":" + Second;
    return live_time;
  }
  return "0";
}
String loggenegrate(String data1) {
  eeprom datetime;
  jio *ptr1 = new jio;
  ptr1->functionManager(5);
  destructTest(ptr1);
  return logdata(data1, datetime.read_bytes(181));

}
String logfileName(String name1) {
  name1.replace('!', '/');
  return name1;
}
