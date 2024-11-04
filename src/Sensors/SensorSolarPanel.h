#ifndef SENSORSOLARPANEL_H
#define SENSORSOLARPANEL_H

#include "MQTTManager.h"
#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#include "Arduino.h"

#include "Sensors/Sensor.h"

#include "ADS1X15.h"

#include "secrets.h"

#include "ADS1115Controller.h"

class SensorSolarPanel : public Sensor {
    
    public:
        SensorSolarPanel(){}

        void run();
    private:

        const uint8_t ADS_INPUT_SOLAR_VOLTAGE = 2;
        const uint8_t ADS_INPUT_SOLAR_CURRENT = 3;

        TaskHandle_t taskSolarHandle = NULL;

        static void taskSolar(void* pvParameters);
        
        volatile bool continueTask = true;

        float getSolarCurrent();
        float getSolarVoltage();
};
#endif

