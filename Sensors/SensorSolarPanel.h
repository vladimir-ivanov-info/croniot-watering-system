#ifndef SENSORSOLARPANEL_H
#define SENSORSOLARPANEL_H

#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"
#include "Sensors/Sensor.h"

#include "ADS1115Controller.h"
#include "secrets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

class SensorSolarPanel : public Sensor {

public:
    SensorSolarPanel() = default;
    void run();

private:
    static constexpr uint8_t ADS_INPUT_SOLAR_VOLTAGE = 2;
    static constexpr uint8_t ADS_INPUT_SOLAR_CURRENT = 3;
    static constexpr int SENSOR_MEASUREMENT_SAMPLES = 20;

    TaskHandle_t taskSolarHandle = nullptr;
    volatile bool continueTask = true;

    static void taskSolar(void* pvParameters);

    float getSolarCurrent();
    float getSolarVoltage();
};

#endif
