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

        static void task(void* pvParameters);
};
#endif

