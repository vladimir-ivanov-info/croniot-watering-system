#ifndef SENSORWIFISTRENGTH_H
#define SENSORWIFISTRENGTH_H

#include "MQTTManager.h"
#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#include <WiFi.h>

#include "Sensors/Sensor.h"
#include "secrets.h"

class SensorWiFiStrength : public Sensor {
    
    public:
        SensorWiFiStrength(){}

        void run() override;
        void stop() {
            continueTask = false;
        }
    private:

        volatile bool continueTask = true;

        TaskHandle_t taskHandle = NULL;

        static void taskFunction(void *param) {
            SensorWiFiStrength *self = (SensorWiFiStrength *)param;

            int sensorUid = static_cast<int>(SENSOR_WIFI_STRENGTH);

            bool continueTask = self->continueTask;

            // Your RTOS task logic goes here
            while (continueTask) {
                
                int measurements = 10;
                long rssi = 0;
                long averageRSSI = 0;
                
                for (int i=0; i < measurements; i++){
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
};
#endif

