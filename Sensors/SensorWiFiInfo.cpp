#include "SensorWiFiInfo.h"

static const char* TAG = "SensorWiFi";

void SensorWiFiStrength::run() {
    xTaskCreate(task, "SensorWiFi", 4096, this, 1, &taskHandle);
}

void SensorWiFiStrength::task(void* pvParameters) {
    auto* self = static_cast<SensorWiFiStrength*>(pvParameters);
    ESP_LOGI(TAG, "Sensor WiFi strength task started");

    while (self->continueTask) {
        wifi_ap_record_t ap_info;

        int rssi_sum = 0;
        int measurements = 10;

        for (int i = 0; i < measurements; ++i) {
            if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
                rssi_sum += ap_info.rssi;
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        int average_rssi = rssi_sum / measurements;
        std::string rssi_str = std::to_string(average_rssi);
   // ESP_LOGI(TAG, "Sensor WiFi strength: %s", rssi_str.c_str());

        self->sendSensorData(std::string(DEVICE_UUID), SENSOR_WIFI_STRENGTH, rssi_str);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "Sensor WiFi strength task stopped");
    vTaskDelete(nullptr);
}
