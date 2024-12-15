#include "SensorCurrentTime.h"

void SensorCurrentTime::run(){
    xTaskCreatePinnedToCore(
        taskCurrentTime,      // Function to run the task
        "SensorCurrentTime",          // Task name
        2048,             // Stack size (words)
        this,              // Task parameter (pass the instance of the singleton)
        1,                 // Task priority
        &taskCurrentTimeHandle,       // Task handle
        0                  // Core to run the task (0 or 1)
    );
}

void SensorCurrentTime::taskCurrentTime(void* pvParameters) {
    SensorCurrentTime* self = static_cast<SensorCurrentTime*>(pvParameters);

    Serial.println("SensorCurrentTime task initialized...");

    while (true) {
        int sensorCurrentTime = static_cast<int>(SENSOR_CURRENT_TIME);

        String topicSensorCurrentTime = "/" + static_cast<String>(DEVICE_UUID) + "/sensor_data/" + String(sensorCurrentTime);

        while(self->continueTask){

                DateTime *dateTime = CurrentDateTimeController::instance().getDateTime(); //TODO clone object and use cloden object's values because original object's values can change while we are initializing the "currentTime" variable

                String currentTime = "";

                currentTime += dateTime->hour;
                currentTime += ":";
                currentTime += dateTime->minute;
                currentTime += ":";
                currentTime += dateTime->second;

                //Serial.print("Current time:"); Serial.println(currentTime);
                
                MQTTManager::instance().publish(topicSensorCurrentTime.c_str(), currentTime.c_str());
                vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        
    }
}