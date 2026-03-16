#ifndef SENSORBATTERY_H
#define SENSORBATTERY_H

#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"
#include "Sensors/Sensor.h"
#include "Sensors/ADS1115Controller.h"
#include "secrets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class SensorBattery : public Sensor {
public:
    SensorBattery() {}

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
    static void taskBattery(void* param);

    volatile bool continueTask = true;

    static float voltsToPercentage(float voltage);

    float getBatteryCurrent();
    float getBatteryVoltage();
};

#endif
