#ifndef SENSORCURRENTTIME_H
#define SENSORCURRENTTIME_H

#include "MQTTManager.h"
#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#include "ADS1X15.h"

#include "Sensors/Sensor.h"
#include "secrets.h"

#include "ADS1115Controller.h"

#include "CurrentDateTimeController.h"

class SensorCurrentTime : public Sensor {

    public:
        SensorCurrentTime(){}

        void run() override;

        void stop() {
            continueTask = false;
            if (taskCurrentTimeHandle != NULL) {
                vTaskDelete(taskCurrentTimeHandle);
                taskCurrentTimeHandle = NULL;
            }
        }

    private:
        TaskHandle_t taskCurrentTimeHandle = NULL;
        static void taskCurrentTime(void *param);

        volatile bool continueTask = true;

};

#endif