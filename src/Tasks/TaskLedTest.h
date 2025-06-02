#ifndef TASKLEDTEST_H
#define TASKLEDTEST_H

#include <Arduino.h>
#include "Sensors/SensorDefs.h"

#include <map>

#include "Tasks/TaskBase.h"
#include "Tasks/TaskData.h"

#include "secrets.h"
#include "Tasks/TaskController.h"

#include "Tasks/SimpleTaskData.h"
#include "Tasks/TaskProgressUpdate.h"

using byte = uint8_t;  // Resolve ambiguity by explicitly defining byte as uint8_t


class TaskLedTest : public TaskBase {

    public:
        TaskLedTest();
        int getUid() override { return TASK_LED_TEST; }

        void loop() override;
        
        void run() override;

        static constexpr int pinLedTest = 27;

    private:
        std::map<int, String> parametersValues;

        struct TaskParams {
            TaskLedTest* obj;
        };

        void executeTask(SimpleTaskData& taskData) override;

};
 #endif