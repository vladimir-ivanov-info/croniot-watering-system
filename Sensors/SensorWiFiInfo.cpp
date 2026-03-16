// #include "SensorWiFiInfo.h"

// void SensorWiFiStrength::run(){
//     xTaskCreatePinnedToCore(
//         task,      // Function to run the task
//         "MyTask",          // Task name
//         10000,             // Stack size (words)
//         this,              // Task parameter (pass the instance of the singleton)
//         1,                 // Task priority
//         &taskHandle,       // Task handle
//         0                  // Core to run the task (0 or 1)
//     );
// }

// void SensorWiFiStrength::task(void* pvParameters) {
//     SensorWiFiStrength *self = (SensorWiFiStrength *)pvParameters;

//     bool continueTask = self->continueTask;

//     // Your RTOS task logic goes here
//     while (continueTask) {
        
//         int measurements = 10;
//         long rssi = 0;
//         long averageRSSI = 0;
        
//         for (int i = 0; i < measurements; i++){
//             rssi += WiFi.RSSI();
//             vTaskDelay(50 / portTICK_PERIOD_MS);
//         }

//         averageRSSI = rssi/measurements;

//         int wifiStrength = averageRSSI;
//         String wifiStrengthStr = String(wifiStrength);
        
//         self->sendSensorData(static_cast<String>(DEVICE_UUID), static_cast<int>(SENSOR_WIFI_STRENGTH), wifiStrengthStr);
//         vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000ms
//         //vTaskDelay(100 / portTICK_PERIOD_MS); // Delay for 1000ms
//     }
//     Serial.println("Sensor WifiSignal stopped");
//     vTaskDelete(NULL);
// }



#include "SensorWiFiInfo.h"

static const char* TAG = "SensorWiFi";

void SensorWiFiStrength::run() {
    /*xTaskCreatePinnedToCore(
        task,
        "SensorWiFi",
        4096,
        this,
        1,
        &taskHandle,
        0
    );*/

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
    ESP_LOGI(TAG, "Sensor WiFi strength: %s", rssi_str.c_str());

        self->sendSensorData(std::string(DEVICE_UUID), SENSOR_WIFI_STRENGTH, rssi_str);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "Sensor WiFi strength task stopped");
    vTaskDelete(nullptr);
}
