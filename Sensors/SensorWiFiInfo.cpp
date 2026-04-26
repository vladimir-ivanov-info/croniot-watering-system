#include "SensorWiFiInfo.h"

static const char* TAG = "SensorWiFi";

void SensorWiFiStrength::run() {
    xTaskCreate(task, "SensorWiFi", 4096, this, 1, &taskHandle);
}

void SensorWiFiStrength::task(void* pvParameters) {
    auto* self = static_cast<SensorWiFiStrength*>(pvParameters);
    ESP_LOGI(TAG, "Sensor WiFi strength task started");

    uint32_t reportCounter = 0;

    while (self->continueTask) {
        wifi_ap_record_t ap_info;

        int rssi_sum = 0;
        int measurements = 10;
        int valid_samples = 0;
        int failed_samples = 0;

        for (int i = 0; i < measurements; ++i) {
            if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
                // Expected RSSI range in dBm for Wi-Fi.
                if (ap_info.rssi <= -20 && ap_info.rssi >= -127) {
                    rssi_sum += ap_info.rssi;
                    valid_samples++;
                } else {
                    failed_samples++;
                }
            } else {
                failed_samples++;
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        if (valid_samples == 0) {
            ESP_LOGW(TAG, "No valid RSSI samples (failed=%d)", failed_samples);
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        int average_rssi = rssi_sum / valid_samples;
        std::string rssi_str = std::to_string(average_rssi);
        reportCounter++;

        // Periodic RF diagnostics to detect wrong BSSID / channel / unstable RSSI.
        if ((reportCounter % 10) == 0) {
            ESP_LOGI(
                TAG,
                "RSSI avg=%d dBm (valid=%d, failed=%d) ap_rssi=%d channel=%u bssid=%02x:%02x:%02x:%02x:%02x:%02x",
                average_rssi,
                valid_samples,
                failed_samples,
                ap_info.rssi,
                ap_info.primary,
                ap_info.bssid[0], ap_info.bssid[1], ap_info.bssid[2],
                ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]
            );
        }

        self->sendSensorData(SENSOR_WIFI_STRENGTH, rssi_str);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "Sensor WiFi strength task stopped");
    vTaskDelete(nullptr);
}
