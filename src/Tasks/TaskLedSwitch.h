#ifndef TASKLEDSWITCH_H
#define TASKLEDSWITCH_H

#include <Arduino.h>
#include "HttpController.h"

#include "Sensors/SensorDefs.h"

#include <map>

#include "MQTTManager.h"
#include "Tasks/TaskBase.h"
#include "Tasks/TaskData.h"

#include "secrets.h"
#include "Tasks/TaskController.h"

#include "Tasks/SimpleTaskData.h"
#include "Tasks/TaskProgressUpdate.h"


class TaskLedSwitch : public TaskBase {

    public:
        TaskLedSwitch();
        int getUid() override { return TASK_LED_SWITCH; }

        void loop() override;
        
        void run() override;

        static constexpr int pinLedTest = 27;

        //static void taskFunction(void* pvParameters);

    private:
        std::map<int, String> parametersValues;

        struct TaskParams {
            TaskLedSwitch* obj;
        };

        void executeTask(SimpleTaskData& taskData) override;

};

#endif