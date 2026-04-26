#include "SensorRamUsage.h"

static const char* TAG = "SensorRamUsage";

void SensorRamUsage::run() {
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
            self->sendSensorData(static_cast<int>(SENSOR_RAM_USAGE), usageRam);

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
