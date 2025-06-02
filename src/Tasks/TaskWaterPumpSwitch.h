#ifndef TASKWATERPUMPSWITCH_H
#define TASKWATERPUMPSWITCH_H

#include <Arduino.h>

#include "Sensors/SensorDefs.h"

#include <map>

#include "Tasks/TaskBase.h"
#include "Tasks/TaskData.h"

#include "secrets.h"
#include "Tasks/TaskController.h"

#include "Tasks/SimpleTaskData.h"
#include "Tasks/TaskProgressUpdate.h"

class TaskWaterPumpSwitch : public TaskBase {

    public:
        TaskWaterPumpSwitch();
        int getUid() override { return TASK_WATER_PUMP_SWITCH; }

        void loop() override;
        
        void run() override;

        static constexpr int pinWaterPump = 4;
        
    private:
        std::map<int, String> parametersValues;

        struct TaskParams {
            TaskWaterPumpSwitch* obj;
        };

        void executeTask(SimpleTaskData& taskData) override;

};

#endif