#include <ESP8266WiFi.h>
#include <EEPROM.h>
class eeprom
{
  public:
    eeprom() {
      Serial.println("eeprom_start");
    }
    ~eeprom() {
      Serial.println("stop");
    }
    char* write_bytes(int position, char* Data, size_t len) {
      if (position >= 0 && len > 1 && len < 10 ) {
        delayMicroseconds(1);
        for (int i = 0; i < 10; i++) {
          if (i < len) {
            EEPROM.write((i + position), Data[i]);
          }
          else {
            EEPROM.write((i + position), 0);
            EEPROM.commit();
            return "done";
          }
        }
      }
      return "error";
    }
    char* read_bytes(int position) {
      char readdata[10];
      for (int i = 0; i < 10; i++) {
        if ((EEPROM.read(i + position) != 0
            ) && (EEPROM.read(i + position) != 255 )) {
          readdata[i] = (char )EEPROM.read(i + position);
          delayMicroseconds(1);
        }
        else {
          readdata[i] = NULL;
          Serial.println((char *) readdata);
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  EEPROM.begin(512);
  eeprom E1;
  E1.write_bytes(0, "s23456", 6);
  Serial.println((char*)E1.read_bytes(0));
  E1.write_bytes(10, "sahul", 5);
  Serial.println((char*)E1.read_bytes(10));
}

void loop() {
  // put your main code here, to run repeatedly:

}
