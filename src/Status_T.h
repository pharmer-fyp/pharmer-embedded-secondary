#include <Arduino.h>
#include <ArduinoJson.h>

class status_T  // Status type
{
 private:
  float airHumidity;
  float airTemp;
  float soilMoisture;

 public:
  status_T() {
    airHumidity = 0;
    airTemp = 0;
    soilMoisture = 0;
  }
  float getAirHumidity() { return airHumidity; }
  float getAirTemp() { return airTemp; }
  float getSoilMoisture() { return soilMoisture; }
  void setAirHumidity(float airHumidity) { this->airHumidity = airHumidity; }
  void setAirTemp(float airTemp) { this->airTemp = airTemp; }
  void setSoilMoisture(float soilMoisture) { this->soilMoisture = soilMoisture; }
};
