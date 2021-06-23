#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <SoftwareSerial.h>

#include "Status_T.h"

#define IN_RANGE 0
#define ABOVE_RANGE 1
#define BELOW_RANGE -1

/*Actuator pin configurations*/
#define fanPin 8
#define lightPin 9
#define waterPin 10

#define DHTPIN 7  // Digital pin
#define DHTTYPE DHT11
/*
        DHT11 module
                left pin is data => DHTPIN
                middle pin is Vcc(3.3v)
                right is GND
 */
#define MOISTUREPIN A0  // ADC pin
/*
        Water Capacitive Sensor
                Vcc(3.3v)
                GND
                Do is NC
                Ao => MOISTUREPIN
 */

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial serialPrimary(3, 4);

int inTolerance(float x, float ref);
void readSensors();
void actions();
void readPrimary();
void sendPrimary();
void parseString(String json, status_T *status);
String makeString(status_T *status);

status_T *sensorStatus;
status_T *referenceStatus;
int isDeleted = 0;

void setup() {
  sensorStatus = new status_T();
  referenceStatus = new status_T();
  serialPrimary.begin(9600);
  Serial.begin(9600);
  /*Sensor configurations*/
  dht.begin();

  /*Actuator configurations*/
  pinMode(fanPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(waterPin, OUTPUT);
}

void loop() {
  while (1) {
    readPrimary();
    if (!isDeleted) {
      readSensors();
      actions();
      sendPrimary();
    }
    delay(5000);
  }
}

/*Change according to actual sensors*/
void readSensors() {
  sensorStatus->setAirHumidity(dht.readHumidity());
  sensorStatus->setAirTemp(dht.readTemperature());
  sensorStatus->setSoilMoisture((1023 - analogRead(MOISTUREPIN)) / 1023);
}

int inTolerance(float x, float ref) {  // 10% of nominal value
  float tol;
  tol = (ref * (1.1)) - ref;
  if (x >= (ref - tol) && x <= (ref + tol)) {
    return IN_RANGE;
  }
  if (x < (ref - tol)) {
    return BELOW_RANGE;
  }
  if (x > (ref + tol)) {
    return ABOVE_RANGE;
  }
  return IN_RANGE;
}

/*Change according to actual actuators*/
void actions() {
  int check;
  int fanScore = 0;
  int waterScore = 0;
  int lightScore = 0;
  check = inTolerance(sensorStatus->getAirHumidity(),
                      referenceStatus->getAirHumidity());
  if (check == ABOVE_RANGE) {
    fanScore++;
    waterScore--;
  } else if (check == BELOW_RANGE) {
    fanScore--;
    waterScore++;
  }

  check =
      inTolerance(sensorStatus->getAirTemp(), referenceStatus->getAirTemp());
  if (check == ABOVE_RANGE) {
    fanScore++;
    lightScore--;
  } else if (check == BELOW_RANGE) {
    fanScore--;
    lightScore++;
  }

  check = inTolerance(sensorStatus->getSoilMoisture(),
                      referenceStatus->getSoilMoisture());
  if (check == ABOVE_RANGE) {
    waterScore--;
  } else if (check == BELOW_RANGE) {
    waterScore++;
  }

  if (fanScore > 0)
    digitalWrite(fanPin, 1);
  else
    digitalWrite(fanPin, 0);

  if (waterScore > 0)
    digitalWrite(waterPin, 1);
  else
    digitalWrite(waterPin, 0);

  if (lightScore > 0)
    digitalWrite(lightPin, 1);
  else
    digitalWrite(lightPin, 0);
}

void parseString(String json, status_T *status) {
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (!error) {
    status->setAirHumidity(doc["airHumidity"]);
    status->setAirTemp(doc["airTemp"]);
    status->setSoilMoisture(doc["soilMoisture"]);
  }
}

String makeString(status_T *status) {
  StaticJsonDocument<128> doc;
  doc["airHumidity"] = status->getAirHumidity();
  doc["airTemp"] = status->getAirTemp();
  doc["soilMoisture"] = status->getSoilMoisture();
  String result;
  serializeJson(doc, result);
  return result;
}

void readPrimary() {
  String tmp;
  if (serialPrimary.available()) {
    tmp = serialPrimary.readString();
    if (tmp != "deleted") {
      parseString(tmp, referenceStatus);
      isDeleted = 0;
    } else {
      isDeleted = 1;
      digitalWrite(fanPin, 0);
      digitalWrite(lightPin, 0);
      digitalWrite(waterPin, 0);
    }
  }
}

void sendPrimary() { serialPrimary.println(makeString(sensorStatus)); }
