// #include "SensorRamUsage.h"

// void SensorRamUsage::run(){
//     xTaskCreatePinnedToCore(
//         taskRamUsage,      // Function to run the task
//         "SensorRamUsage",          // Task name
//         4096,             // Stack size (words)
//         this,              // Task parameter (pass the instance of the singleton)
//         1,                 // Task priority
//         &taskRamUsageHandle,       // Task handle
//         0                  // Core to run the task (0 or 1)
//     );
// }

// void SensorRamUsage::taskRamUsage(void* pvParameters) {
//     SensorRamUsage* self = static_cast<SensorRamUsage*>(pvParameters);

//     Serial.println("RamUsage initialized...");

//     uint32_t totalHeapSize = ESP.getHeapSize()/1024;

//     while (true) {
//         while(self->continueTask){
            
//             uint32_t freeHeap = ESP.getFreeHeap()/1024;
//             String usageRam = String(totalHeapSize - freeHeap);

//             self->sendSensorData(static_cast<String>(DEVICE_UUID), static_cast<int>(SENSOR_RAM_USAGE), usageRam);
//             vTaskDelay(1000 / portTICK_PERIOD_MS);
//             //esp_task_wdt_reset();
//         }
//     }
// }

#include "SensorRamUsage.h"

static const char* TAG = "SensorRamUsage";

void SensorRamUsage::run() {
    /*xTaskCreatePinnedToCore(
        taskRamUsage,
        "SensorRamUsage",
        4096,
        this,
        1,
        &taskRamUsageHandle,
        0
    );*/

    xTaskCreate(taskRamUsage, "SensorRamUsage", 4096, this, 1, &taskRamUsageHandle);
}

void SensorRamUsage::stop() {
    continueTask = false;
    if (taskRamUsageHandle != nullptr) {
        vTaskDelete(taskRamUsageHandle);
        taskRamUsageHandle = nullptr;
    }
}

void SensorRamUsage::taskRamUsage(void* pvParameters) {
    auto* self = static_cast<SensorRamUsage*>(pvParameters);

    ESP_LOGI(TAG, "SensorRamUsage initialized...");

    const uint32_t totalHeapSize = heap_caps_get_total_size(MALLOC_CAP_DEFAULT) / 1024;

    while (true) {
        while (self->continueTask) {
            uint32_t freeHeap = esp_get_free_heap_size() / 1024;
            uint32_t usedHeap = totalHeapSize - freeHeap;

            std::string usageRam = std::to_string(usedHeap);
        //    ESP_LOGI(TAG, "Usage RAM: %s", usageRam.c_str());
            self->sendSensorData(std::string(DEVICE_UUID), static_cast<int>(SENSOR_RAM_USAGE), usageRam);

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
