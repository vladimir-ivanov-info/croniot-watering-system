#ifndef SENSORBATTERYVOLTAGE_H
#define SENSORBATTERYVOLTAGE_H

#include "MQTTManager.h"
#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#include "secrets.h"

class SensorBatteryVoltage{
    
    public:
        SensorBatteryVoltage(){}

        void run();
    private:

        TaskHandle_t taskHandle = NULL;

        static void taskFunction(void *param) {
            SensorBatteryVoltage *self = (SensorBatteryVoltage *)param;

            int VOLTAGE_PIN = 34;


            int sensorUid = static_cast<int>(SENSOR_BATTERY_PERCENTAGE);

            pinMode(VOLTAGE_PIN, INPUT);

            for (;;) {
                
              double resultado = 0.0;
              for(int i = 0; i < 1000; i++){
                  int sensorValue = analogRead(VOLTAGE_PIN);
                  resultado += sensorValue;
                  vTaskDelay(5 / portTICK_PERIOD_MS);
              }

              resultado = resultado/1000;

              double batteryVoltage  = resultado * 14.26 / 4096;

              String batteryVoltageStr = String(batteryVoltage);

              String topic = "/" + static_cast<String>(DEVICE_UUID) + "/sensor_data/" + String(sensorUid);
                  
              MQTTManager::instance().getClient()->publish(topic.c_str(), batteryVoltageStr.c_str());
              vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000ms
              //self->publish();
              // self->getClient().flush();
              // self->getClient().loop();
            }
        }
};
#endif

