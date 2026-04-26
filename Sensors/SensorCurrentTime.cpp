#include "SensorCurrentTime.h"
#include "esp_log.h"

static const char* TAG = "SensorCurrentTime";

void SensorCurrentTime::run() {
    xTaskCreate(taskCurrentTime, "SensorCurrentTime", 4096, this, 1, &taskCurrentTimeHandle);
}

void SensorCurrentTime::stop() {
    continueTask = false;
    if (taskCurrentTimeHandle != nullptr) {
        vTaskDelete(taskCurrentTimeHandle);
        taskCurrentTimeHandle = nullptr;
    }
}

void SensorCurrentTime::taskCurrentTime(void* pvParameters) {
    auto* self = static_cast<SensorCurrentTime*>(pvParameters);

    ESP_LOGI(TAG, "SensorCurrentTime initialized...");

    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(1000);

    while (true) {
        if (self->continueTask) {
            DateTime* dateTime = CurrentDateTimeController::instance().getDateTime();

            // Construir string con C++ std::string
            char buffer[16];
            snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", dateTime->hour, dateTime->minute, dateTime->second);
            std::string currentTime(buffer);


            ESP_LOGI(TAG, "Time: %s", currentTime.c_str());

            self->sendSensorData(static_cast<int>(SENSOR_CURRENT_TIME), currentTime);

            // Keep a stable 1 Hz cadence even if publishing introduces jitter.
            vTaskDelayUntil(&lastWakeTime, period);
        } else {
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}
