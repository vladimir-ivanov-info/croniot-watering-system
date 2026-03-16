#ifndef SENSORRAMUSAGE_H
#define SENSORRAMUSAGE_H

#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"
#include "Sensors/Sensor.h"
#include "secrets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

class SensorRamUsage : public Sensor {

public:
    SensorRamUsage() = default;
    void run() override;
    void stop();

private:
    volatile bool continueTask = true;
    TaskHandle_t taskRamUsageHandle = nullptr;

    static void taskRamUsage(void* pvParameters);
};

#endif
