#include "SensorBattery.h"

void SensorBattery::run(){
    xTaskCreatePinnedToCore(
        taskFunction,      // Function to run the task
        "SensorBattery",          // Task name
        10000,             // Stack size (words)
        this,              // Task parameter (pass the instance of the singleton)
        1,                 // Task priority
        &taskHandle,       // Task handle
        0                  // Core to run the task (0 or 1)
    );
}
