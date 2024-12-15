#include "SensorsInitializer.h"

void SensorsInitializer::initSensors(){
    registerSensorTypes();
    registerSensors();
}

void SensorsInitializer::registerSensorTypes(){
  //2
  std::map<String, String> constraints3 = {{PARAMETERS_NUMBER_MIN_VALUE, "-90"}, {PARAMETERS_NUMBER_MAX_VALUE, "-30"}};
  SensorType *sensor2 = new SensorType(static_cast<String>(SENSOR_WIFI_STRENGTH), "WiFi signal", "WiFi signal level expressed in dBm", Parameter(1, "WiFi signal", PARAMETERS_NUMBER, "WiFi signal level expressed in dBm", "dBm", constraints3));

  //3
  std::map<String, String> constraints4 = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, "100"}};
  SensorType *sensor3 = new SensorType(static_cast<String>(SENSOR_BATTERY_PERCENTAGE), "Battery level", "Battery level measured in percents", Parameter(1, "Battery level", PARAMETERS_NUMBER, "Battery level measured in percents", "%", constraints4));

  //4
  std::map<String, String> constraints5 = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, "96"}};
  SensorType *sensor4 = new SensorType(static_cast<String>(SENSOR_BATTERY_POWER_CONSUMPTION), "Battery power", "Main battery power at a given moment", Parameter(1, "Battery power", PARAMETERS_NUMBER, "Main battery power at a given moment", "W", constraints5));

  //5
  std::map<String, String> constraints6 = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, "50"}};
  SensorType *sensor5 = new SensorType(static_cast<String>(SENSOR_SOLAR_POWER), "Solar power", "Solar panel power at a given moment", Parameter(1, "Solar power", PARAMETERS_NUMBER, "Solar panel power at a given moment", "W", constraints6));

  std::map<String, String> constraints7 = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, String(ESP.getHeapSize()/1024)}};
  SensorType *sensor6 = new SensorType(static_cast<String>(SENSOR_RAM_USAGE), "RAM usage", "RAM memory usage", Parameter(1, "RAM usage", PARAMETERS_NUMBER, "RAM usage at a given moment", "KB", constraints7));

  std::map<String, String> constraints8; // = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, String(ESP.getHeapSize()/1024)}};
  SensorType *sensorCurrentTime = new SensorType(static_cast<String>(SENSOR_CURRENT_TIME), "System time", "Current device's time", Parameter(1, "System time", PARAMETERS_STRING, "Current device's time", "", constraints8));


  SensorsController::instance().addSensorType(sensor2);
  SensorsController::instance().addSensorType(sensor3);
  SensorsController::instance().addSensorType(sensor4);
  SensorsController::instance().addSensorType(sensor5);
  SensorsController::instance().addSensorType(sensor6);
  SensorsController::instance().addSensorType(sensorCurrentTime);
}

void SensorsInitializer::registerSensors(){
  SensorsController::instance().addSensor(new SensorWiFiStrength());
  SensorsController::instance().addSensor(new SensorBattery());
  SensorsController::instance().addSensor(new SensorSolarPanel());
  SensorsController::instance().addSensor(new SensorRamUsage());
  SensorsController::instance().addSensor(new SensorCurrentTime());

}