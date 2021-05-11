#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DallasTemperature.h>
#include <OneWire.h>

/*Actuator pin configurations*/
#define fanPin 8
#define lightPin 9
#define waterPin 10

#define DHTPIN 7 //Digital pin
#define DHTTYPE DHT11
/*
	DHT11 module	
 		left pin is data => DHTPIN
		middle pin is Vcc(3.3v)
		right is GND
 */
#define MOISTUREPIN A0 //ADC pin
/*
 	Water Capacitive Sensor
 		Vcc(3.3v)
 		GND
 		Do is NC
 		Ao => MOISTUREPIN
 */
#define SOILTEMPPIN 6 //Digital pin
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

void readSensors();
void actions();

class status_T  // Status type
{
 public:
  float airHumidity;
  float airTemp;
  float soilMoisture;
  float soilTemp;

  status_T() {
    airHumidity = 0;
    airTemp = 0;
    soilMoisture = 0;
    soilTemp = 0;
  }

  void parseString(String str){
    //TODO: convert string to values
    //string in json format?
  }

  String makeString(){
    //TODO: convert values to string
  }

  //TODO: method to read from poweron to prevent dataloss from power outage
  //  Possible: eeprom read/write

};

status_T *currStatus;
status_T *idealStatus;


void setup() {
  currStatus = new status_T();
  idealStatus = new status_T();
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
  if (serialPrimary.available()) {
    idealStatus->parseString(serialPrimary.readString());
  }
  readSensors();
  actions();
  serialPrimary.print(currStatus->makeString());
  delay(500);
}

/*Change according to actual sensors*/
void readSensors() {
  currStatus->airHumidity = dht.readHumidity();
  currStatus->airTemp = dht.readTemperature();
  currStatus->soilMoisture = (1023 - analogRead(MOISTUREPIN))/ 1023;
  currStatus->soilTemp = sensors.getTempCByIndex(0);
}

/*Change according to actual actuators*/
void actions() {
  if (currStatus->airHumidity > idealStatus->airHumidity){
    //TODO
  } else {
    //TODO
  }

  if (currStatus->airTemp > idealStatus->airTemp){
    //TODO
  } else {
    //TODO
  }

  if (currStatus->soilMoisture > idealStatus->soilMoisture){
    //TODO
  } else {
    //TODO
  }

  if (currStatus->soilTemp > idealStatus->soilTemp){
    //TODO
  } else {
    //TODO
  }
}