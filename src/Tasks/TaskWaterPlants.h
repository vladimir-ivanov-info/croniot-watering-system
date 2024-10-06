#ifndef TASKWATERPLANTS_H
#define TASKWATERPLANTS_H

#include <Arduino.h>
#include "Global.h"
#include "HttpController.h"

#include "Sensors/SensorDefs.h"

#include "MQTTManager.h"

#include <map>

class TaskWaterPlants{

    public:
       // TaskWaterPlants(std::map<int, String> parametersValues);
        TaskWaterPlants(int taskUid, const std::map<int, String>& parametersValues);

        void run();

        static constexpr int pinWaterPump = 4;
        static constexpr int pinElectricValve1 = 5;
        static constexpr int pinElectricValve2 = 18;

        const byte relayOnState = LOW;
        const byte relayOffState = HIGH;

    private:

      //  int pinWaterPump = 4;
     //   int pinElectricValve1 = 5;
     //   int pinElectricValve2 = 18;
        int taskUid;
        std::map<int, String> parametersValues;

        static void run2(void *parameter);

        struct TaskParams {
            TaskWaterPlants* obj;
        };

};
 #endif