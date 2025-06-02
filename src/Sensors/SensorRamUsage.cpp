#include "SensorRamUsage.h"

void SensorRamUsage::run(){
    xTaskCreatePinnedToCore(
        taskRamUsage,      // Function to run the task
        "SensorRamUsage",          // Task name
        4096,             // Stack size (words)
        this,              // Task parameter (pass the instance of the singleton)
        1,                 // Task priority
        &taskRamUsageHandle,       // Task handle
        0                  // Core to run the task (0 or 1)
    );
}

void SensorRamUsage::taskRamUsage(void* pvParameters) {
    SensorRamUsage* self = static_cast<SensorRamUsage*>(pvParameters);

    Serial.println("RamUsage initialized...");

    uint32_t totalHeapSize = ESP.getHeapSize()/1024;

    while (true) {
        while(self->continueTask){
            
            uint32_t freeHeap = ESP.getFreeHeap()/1024;
            String usageRam = String(totalHeapSize - freeHeap);

            self->sendSensorData(static_cast<String>(DEVICE_UUID), static_cast<int>(SENSOR_RAM_USAGE), usageRam);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            //esp_task_wdt_reset();
        }
    }
}