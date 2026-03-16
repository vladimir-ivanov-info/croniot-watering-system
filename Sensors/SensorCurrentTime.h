#ifndef SENSORCURRENTTIME_H
#define SENSORCURRENTTIME_H

#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"
#include "Sensors/Sensor.h"
#include "secrets.h"

#include "ADS1115Controller.h"
#include "CurrentDateTimeController.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class SensorCurrentTime : public Sensor {

public:
    SensorCurrentTime() = default;
    void run() override;
    void stop();

private:
    TaskHandle_t taskCurrentTimeHandle = nullptr;
    static void taskCurrentTime(void *param);
    volatile bool continueTask = true;
};

#endif
