#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial serialPrimary(3, 4);

void readSensors();
void actions();

String currStatus;
String idealStatus;
// Format of Status String: "xy"
//                       x => status of button 1
//                       y => status of button 2

void setup() {
  serialPrimary.begin(9600);
  Serial.begin(9600);
  /*Test Mode configurations*/
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  /*Change according to actual string*/
  /*Maybe read from eeprom to avoid dataloss on poweroff*/
  idealStatus = "11";
}

void loop() {
  if (serialPrimary.available()) {
    idealStatus = serialPrimary.readString();
    Serial.print(idealStatus);
  }
  readSensors();
  actions();
  serialPrimary.print(currStatus + "\n");
  delay(500);
}

/*Change according to actual sensors*/
void readSensors() {
  int button1 = digitalRead(6);
  int button2 = digitalRead(7);
  currStatus = "";
  currStatus += (char)(48+button1);
  currStatus += (char)(48+button2);
}

/*Change according to actual actuators*/
void actions() {
  int button1 = currStatus[0];
  int button2 = currStatus[1];
  int idealButton1 = idealStatus[0];
  int idealButton2 = idealStatus[1];

  if (button1 != idealButton1) {
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
  }
  if (button2 != idealButton2) {
    digitalWrite(9, HIGH);
  } else {
    digitalWrite(9, LOW);
  }
}