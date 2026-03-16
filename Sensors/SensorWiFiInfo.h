#ifndef SENSORWIFIINFO_H
#define SENSORWIFIINFO_H

#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"
#include "Sensors/Sensor.h"
#include "secrets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

class SensorWiFiStrength : public Sensor {
public:
    SensorWiFiStrength() = default;
    void run() override;
    void stop() { continueTask = false; }

private:
    volatile bool continueTask = true;
    TaskHandle_t taskHandle = nullptr;

    static void task(void* pvParameters);
};

#endif
