#include <Arduino.h>
#include "secrets.h"
#include "CommonSetup.h"
#include "NetworkManager.h"
#include "secrets.h" //TODO create this file as explained in secrets_mock.h
#include "CommonConstants.h"
#include "TasksInitializer.h"
#include "SensorsInitializer.h"

void setServer();
void setupCredentialsAndConnectToServer();

String DEVICE_UUID_EXTERN = DEVICE_UUID;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  esp_task_wdt_init(10, true); // Set timeout to 10 seconds
  esp_task_wdt_add(NULL);      // Add loopTask to WDT monitoring

  setServer();

  NetworkManager::instance().setWifiCredentials(Secrets::WIFI_SSID, Secrets::WIFI_PASSWORD);
  NetworkManager::instance().setServerAddress(Secrets::SERVER_ADDRESS, Secrets::SERVER_PORT, Secrets::SERVER_PORT_MQTT);

  //Registration internal to IoT device. After the connection is established with the server, the device will send this information to said server if necessary.
  SensorsInitializer::initSensors();
  TasksInitializer::initTasks();

  setupCredentialsAndConnectToServer();
}

void setServer(){

  const int PIN_SERVER_SELECTION = 33;

  pinMode(PIN_SERVER_SELECTION, INPUT);
  int pinServeState = digitalRead(PIN_SERVER_SELECTION);

  if(pinServeState == HIGH){
    Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_REMOTE;
  } else {
    Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_LOCAL;
  }

  Serial.print("### Current server: "); Serial.println(Secrets::SERVER_ADDRESS);
}

void setupCredentialsAndConnectToServer(){
  UserCredentials actualCredentials = UserCredentials(ACCOUNT_EMAIL, ACCOUNT_UUID, ACCOUNT_PASSWORD, DEVICE_UUID, "", DEVICE_NAME, DEVICE_DESCTIPION);
  CommonSetup::instance().setup(actualCredentials);
}

void loop() {
  CommonSetup::instance().loop();
}
