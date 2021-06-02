#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DallasTemperature.h>
#include <OneWire.h>
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
#define SOILTEMPPIN 6  // Digital pin
/*
        Soil temperature Sensor
                red is Vcc(3.3v)
                black is GND
                green is data
                        green => resistor => Vcc
                              => SOILTEMPPIN

 */

OneWire oneWire(SOILTEMPPIN);
DallasTemperature sensors(&oneWire);
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

void setup() {
  sensorStatus = new status_T();
  referenceStatus = new status_T();
  serialPrimary.begin(9600);
  Serial.begin(9600);
  /*Sensor configurations*/
  dht.begin();
  sensors.begin();

  /*Actuator configurations*/
  pinMode(fanPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(waterPin, OUTPUT);
}

void loop() {
  while (1) {
    readPrimary();
    readSensors();
    actions();
    sendPrimary();
    delay(500);
  }
}

/*Change according to actual sensors*/
void readSensors() {
  sensorStatus->setAirHumidity(dht.readHumidity());
  sensorStatus->setAirTemp(dht.readTemperature());
  sensorStatus->setSoilMoisture((1023 - analogRead(MOISTUREPIN)) / 1023);
  sensorStatus->setSoilTemp(sensors.getTempCByIndex(0));
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
}

/*Change according to actual actuators*/
void actions() {
  int check;

  check = inTolerance(sensorStatus->getAirHumidity(),
                      referenceStatus->getAirHumidity());
  if (check == ABOVE_RANGE) {
    // turn fan on
    // turn water off
  } else if (check == BELOW_RANGE) {
    // turn fan off
    // turn water on
  }

  check = inTolerance(sensorStatus->getAirTemp(),
                      referenceStatus->getAirTemp());
  if (check == ABOVE_RANGE) {
    // turn fan on
    // turn lights off
  } else if (check == BELOW_RANGE) {
    // turn fan off
    // turn lights on
  }

  check = inTolerance(sensorStatus->getSoilMoisture(),
                      referenceStatus->getSoilMoisture());
  if (check == ABOVE_RANGE) {
    // turn water off
  } else if (check == BELOW_RANGE) {
    // turn water on
  }

  check = inTolerance(sensorStatus->getSoilTemp(),
                      referenceStatus->getSoilTemp());
  if (check == ABOVE_RANGE) {
    // turn fan on
    // turn water on
  } else if (check == BELOW_RANGE) {
    // turn fan off
    // turn water off
  }
}

void parseString(String json, status_T *status) {
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (!error) {
    status->setAirHumidity(doc["airHumidity"]);
    status->setAirTemp(doc["airTemp"]);
    status->setSoilMoisture(doc["soilMoisture"]);
    status->setSoilTemp(doc["soilTemp"]);
  }
}

String makeString(status_T *status) {
  StaticJsonDocument<128> doc;
  doc["airHumidity"] = status->getAirHumidity();
  doc["airTemp"] = status->getAirTemp();
  doc["soilMoisture"] = status->getSoilMoisture();
  doc["soilTemp"] = status->getSoilTemp();
  String result;
  serializeJson(doc, result);
  return result;
}

void readPrimary() {
  if (serialPrimary.available()) {
    parseString(serialPrimary.readString(), referenceStatus);
  }
}

void sendPrimary() { serialPrimary.print(makeString(sensorStatus)); }
