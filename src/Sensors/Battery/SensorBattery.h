#ifndef SENSORBATTERY_H
#define SENSORBATTERY_H

#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#include "ADS1X15.h"

#include "Sensors/Sensor.h"
#include "secrets.h"

#include "Sensors/ADS1115Controller.h"

class SensorBattery : public Sensor {
    
    public:
        SensorBattery(){}

        void run() override;

        void stop() {
            continueTask = false;
            if (taskBatteryHandle != NULL) {
                vTaskDelete(taskBatteryHandle);
                taskBatteryHandle = NULL;
            }
        }

    private:

        const uint8_t ADS_INPUT_BATTERY_CURRENT = 0;
        const uint8_t ADS_INPUT_BATTERY_VOLTAGE = 1;

        const int SENSOR_MESAUREMENT_SAMPLES = 20;


        TaskHandle_t taskBatteryHandle = NULL;
        static void taskBattery(void *param);


        volatile bool continueTask = true;

        static float voltsToPercentage(String voltsStr);

        float getBatteryCurrent();
        float getBatteryVoltage();
};
#endif

