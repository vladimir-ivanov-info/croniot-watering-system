#include <Arduino.h>

#include "secrets.h"

#include "CommonSetup.h"
#include "NetworkManager.h"

#include "Tasks/TaskController.h"
#include "Messages/MessageRegisterSensorType.h"
#include "Sensors/SensorDefs.h"

#include "Sensors/SensorsController.h"
#include "Tasks/TaskController.h"


#include "Sensors/SensorWiFiStrength.h"
#include "Sensors/SensorBattery.h"
#include "Sensors/SensorSolarPanel.h"

#include "Tasks/TaskLedTest.h"
#include "Tasks/TaskLedSwitch.h"
#include "Tasks/TaskWaterPlants.h"

#include "secrets.h"
#include "Storage.h"

#include <map>

#include "CommonConstants.h"

static constexpr int pinWaterPump = 4;

void registerSensorTypes();
void registerSensors();
void registerTaskTypes();
void registerTasks();
void setupCredentialsAndConnectToServer();

String DEVICE_UUID_EXTERN = DEVICE_UUID;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  
  pinMode(pinWaterPump, OUTPUT);
  digitalWrite(pinWaterPump, HIGH); //HIGH = off for this relay

  int pinElectricValve1 = 5;
  int pinElectricValve2 = 18;

  pinMode(pinElectricValve1, OUTPUT);
  pinMode(pinElectricValve2, OUTPUT);

  digitalWrite(pinElectricValve1, LOW);
  digitalWrite(pinElectricValve2, LOW);

  String serverAddress = static_cast<String>(SERVER_ADDRESS);
  uint16_t serverPort = static_cast<uint16_t>(SERVER_PORT);
  uint16_t serverMqttPort = static_cast<uint16_t>(SERVER_PORT_MQTT);

  Serial.println("PASSWORD: [");
  Serial.println(static_cast<String>(WIFI_PASSWORD));
  Serial.println("]");

  NetworkManager::instance().setWifiCredentials(static_cast<String>(WIFI_SSID), static_cast<String>(WIFI_PASSWORD));
  NetworkManager::instance().setServerAddress(serverAddress, serverPort, serverMqttPort);

  //Registration internal to IoT device. After the connection is established with the server, the device will send this information to said server if necessary.
  registerSensorTypes();
  registerSensors();
  registerTaskTypes();
  registerTasks();

  setupCredentialsAndConnectToServer();
}

void setupCredentialsAndConnectToServer(){
  UserCredentials credentialsInMemory = Storage::instance().readUserCredentials();

  UserCredentials actualCredentials = UserCredentials(ACCOUNT_EMAIL, ACCOUNT_UUID, ACCOUNT_PASSWORD, DEVICE_UUID, "", DEVICE_NAME, DEVICE_DESCTIPION);
  CommonSetup::instance().setup(actualCredentials);
}

void loop() {
  CommonSetup::instance().loop();
}

void registerSensorTypes(){

  //2
  std::map<String, String> parameters2 = {{PARAMETERS_NUMBER_MIN_VALUE, "-85"}, {PARAMETERS_NUMBER_MAX_VALUE, "-20"}};
  SensorType *sensor2 = new SensorType(static_cast<String>(SENSOR_WIFI_STRENGTH), "WiFi signal", "WiFi signal level expressed in dBm", Parameter(1, "WiFi signal", PARAMETERS_NUMBER, "WiFi signal level expressed in dBm", "dBm", parameters2));

  //3
  std::map<String, String> parameters3 = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, "100"}};
  SensorType *sensor3 = new SensorType(static_cast<String>(SENSOR_BATTERY_PERCENTAGE), "Battery level", "Battery level measured in percents", Parameter(1, "Battery level", PARAMETERS_NUMBER, "Battery level measured in percents", "%", parameters3));

  //4
  std::map<String, String> parameters4 = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, "96"}};
  SensorType *sensor4 = new SensorType(static_cast<String>(SENSOR_BATTERY_POWER_CONSUMPTION), "Battery power", "Main battery power at a given moment", Parameter(1, "Battery power", PARAMETERS_NUMBER, "Main battery power at a given moment", "W", parameters4));

  //5
  std::map<String, String> parameters5 = {{PARAMETERS_NUMBER_MIN_VALUE, "0"}, {PARAMETERS_NUMBER_MAX_VALUE, "100"}};
  SensorType *sensor5 = new SensorType(static_cast<String>(SENSOR_SOLAR_POWER), "Solar power", "Solar panel power at a given moment", Parameter(1, "Solar power", PARAMETERS_NUMBER, "Solar panel power at a given moment", "W", parameters5));

  SensorsController::instance().addSensorType(sensor2);
  SensorsController::instance().addSensorType(sensor3);
  SensorsController::instance().addSensorType(sensor4);
  SensorsController::instance().addSensorType(sensor5);
}

void registerSensors(){
  SensorsController::instance().addSensor(new SensorWiFiStrength());
  SensorsController::instance().addSensor(new SensorBattery());
  SensorsController::instance().addSensor(new SensorSolarPanel());
}

void registerTaskTypes(){
  //WATER PLANTS
  std::map<String, String> constraints;
  constraints.insert({String("minValue"), String("1")});
  constraints.insert({String("maxValue"), String("600")});
  constraints.insert({String("decimals"), String("0")});

  Parameter parameter1(static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION), "Duration", "number", "Duration of the watering in seconds", "s", constraints); 

  list<Parameter> parameters1;
  parameters1.push_back(parameter1);

  std::map<String, String> constraintsWaterPlants2; //no constraints
  Parameter parameter2WaterPlants(static_cast<int>(COMMON_TASK_PARAMETER_TIME), "Time", "time", "Time when you want the task to start", "HH:mm", constraintsWaterPlants2); 
  parameters1.push_back(parameter2WaterPlants);

  TaskType taskType1(static_cast<int>(TASK_WATER_PLANTS), "Water plants", "Water plants for given amount of seconds", parameters1);

  //LED TEST
  std::map<String, String> constraintsLedTest;
  constraintsLedTest.insert({String("minValue"), String("1")});
  constraintsLedTest.insert({String("maxValue"), String("10")});

  Parameter parameter1LedTest(static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION), "Duration", "number", "Duration of the LED turned on, in seconds", "s", constraintsLedTest); 
  list<Parameter> parameters1LedTest;
  parameters1LedTest.push_back(parameter1LedTest);


  std::map<String, String> constraintsLedTest2; //no constraints
  Parameter parameter2LedTest(static_cast<int>(COMMON_TASK_PARAMETER_TIME), "Time", "time", "Time when you want the task to start", "HH:mm", constraintsLedTest2); 
  parameters1LedTest.push_back(parameter2LedTest);

  TaskType taskType2(static_cast<int>(TASK_LED_TEST), "LED test", "Turn on the test LED for given amount of seconds", parameters1LedTest);


  std::map<String, String> constraintsLedSwitch;
  constraintsLedSwitch.insert({String("state_1"), String("on")});
  constraintsLedSwitch.insert({String("state_2"), String("off")});
  Parameter parameter1LedSwitch(static_cast<int>(TASK_LED_SWITCH_PARAMETER_SWITCH_STATE), "LED Switch", "stateful", "Switch to turn on the LED", "", constraintsLedSwitch); 

  list<Parameter> parametersLedSwitch;
  parametersLedSwitch.push_back(parameter1LedSwitch);

  TaskType taskTypeLedSwitch(static_cast<int>(TASK_LED_SWITCH), "LED switch", "Turn on and off the test LED", parametersLedSwitch);

  TaskController::instance().addTaskType(taskType1);
  TaskController::instance().addTaskType(taskType2);
  TaskController::instance().addTaskType(taskTypeLedSwitch);

  TaskController::instance().init();
}

void registerTasks(){
  TaskController::instance().addTask(new TaskLedTest());
  TaskController::instance().addTask(new TaskLedSwitch());
  TaskController::instance().addTask(new TaskWaterPlants());
}