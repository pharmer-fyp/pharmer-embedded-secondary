#include <Arduino.h>
#include <ArduinoJson.h>

class status_T  // Status type
{
 private:
  float airHumidity;
  float airTemp;
  float soilMoisture;
  float soilTemp;

 public:
  status_T() {
    airHumidity = 0;
    airTemp = 0;
    soilMoisture = 0;
    soilTemp = 0;
  }
  float getAirHumidity() { return airHumidity; }
  float getAirTemp() { return airTemp; }
  float getSoilMoisture() { return soilMoisture; }
  float getSoilTemp() { return soilTemp; }
  void setAirHumidity(float airHumidity) { this->airHumidity = airHumidity; }
  void setAirTemp(float airTemp) { this->airTemp = airTemp; }
  void setSoilMoisture(float soilMoisture) { this->soilMoisture = soilMoisture; }
  void setSoilTemp(float soilTemp) { this->soilTemp = soilTemp; }
  // TODO: method to read from poweron to prevent dataloss from power outage
  // Possible: eeprom read/write
};
