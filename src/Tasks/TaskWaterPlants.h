#ifndef TASKWATERPLANTS_H
#define TASKWATERPLANTS_H

#include <Arduino.h>

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

        int getUid() override { return TASK_WATER_PLANTS; }

        void loop() override;

        void run() override;

        static constexpr int pinWaterPump = 4;
        static constexpr int pinElectricValve1 = 5;
        static constexpr int pinElectricValve2 = 18;

        static constexpr uint8_t relayOnState = LOW;
        static constexpr uint8_t relayOffState = HIGH;


    private:

      void openWaterValve();
      void closeWaterValve();
      void startWatering();
      void stopWatering();

      //  int pinWaterPump = 4;
     //   int pinElectricValve1 = 5;
     //   int pinElectricValve2 = 18;
       // int taskUid;
        std::map<int, String> parametersValues;

        //static void run2(void *parameter);

        struct TaskParams {
            TaskWaterPlants* obj;
        };

        //void handleCallback(const String& topic, byte* payload, unsigned int length) override;
        //void executeTask(std::map<int, String> parameters) override;
        void executeTask(SimpleTaskData& taskData) override;
};
 #endif