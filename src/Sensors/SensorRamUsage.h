#ifndef SENSORRAMUSAGE_H
#define SENSORRAMUSAGE_H

#include "MQTTManager.h"
#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#include <WiFi.h>

#include "Sensors/Sensor.h"
#include "secrets.h"

#include "esp_task_wdt.h"

class SensorRamUsage : public Sensor {
    
    public:
        SensorRamUsage(){}

        void run() override;
        void stop() {
            continueTask = false;
        }
        
    private:
        volatile bool continueTask = true;
        
        TaskHandle_t taskRamUsageHandle = NULL;

        static void taskRamUsage(void* pvParameters);

};
#endif

