#include "SensorWiFiStrength.h"

void SensorWiFiStrength::run(){
    xTaskCreatePinnedToCore(
        task,      // Function to run the task
        "MyTask",          // Task name
        10000,             // Stack size (words)
        this,              // Task parameter (pass the instance of the singleton)
        1,                 // Task priority
        &taskHandle,       // Task handle
        0                  // Core to run the task (0 or 1)
    );
}

void SensorWiFiStrength::task(void* pvParameters) {
    SensorWiFiStrength *self = (SensorWiFiStrength *)pvParameters;

    int sensorUid = static_cast<int>(SENSOR_WIFI_STRENGTH);

    bool continueTask = self->continueTask;

    // Your RTOS task logic goes here
    while (continueTask) {
        
        int measurements = 10;
        long rssi = 0;
        long averageRSSI = 0;
        
        for (int i = 0; i < measurements; i++){
            rssi += WiFi.RSSI();
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        averageRSSI = rssi/measurements;

        int wifiStrength = averageRSSI;
        String wifiStrengthStr = String(wifiStrength);

        //Serial.print("WiFi level: "); Serial.print(wifiStrengthStr); Serial.println(" dBm");
        MessageSensorData messageSensorData(sensorUid, wifiStrengthStr);
        String topic = "/" +  static_cast<String>(DEVICE_UUID) + "/sensor_data/" + String(sensorUid);
        String message = messageSensorData.toString();
        
        MQTTManager::instance().publish(topic.c_str(), wifiStrengthStr.c_str());
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000ms
    }
    Serial.println("Sensor WifiSignal stopped");
    vTaskDelete(NULL);
}