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

      std::map<int, String> parametersValues;

      struct TaskParams {
          TaskWaterPlants* obj;
      };

      void executeTask(SimpleTaskData& taskData) override;
};
 #endif