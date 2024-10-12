#ifndef TASKWATERPLANTS_H
#define TASKWATERPLANTS_H

#include <Arduino.h>
#include "Global.h"
#include "HttpController.h"

#include "Sensors/SensorDefs.h"

#include "MQTTManager.h"

#include <map>
#include "secrets.h"

class TaskWaterPlants : public TaskBase {

    public:
       // TaskWaterPlants(std::map<int, String> parametersValues);
      //  TaskWaterPlants(int taskUid, const std::map<int, String>& parametersValues);
        TaskWaterPlants();

        void loop() override;

        void run() override;

        static constexpr int pinWaterPump = 4;
        static constexpr int pinElectricValve1 = 5;
        static constexpr int pinElectricValve2 = 18;

        static constexpr byte relayOnState = LOW;
        static constexpr byte relayOffState = HIGH;

    private:

      //  int pinWaterPump = 4;
     //   int pinElectricValve1 = 5;
     //   int pinElectricValve2 = 18;
       // int taskUid;
        std::map<int, String> parametersValues;

        static void run2(void *parameter);

        struct TaskParams {
            TaskWaterPlants* obj;
        };

        void handleCallback(const String& topic, byte* payload, unsigned int length) override;
        void executeTask(TaskBase::TaskData taskData) override;

};
 #endif