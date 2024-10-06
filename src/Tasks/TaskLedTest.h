#ifndef TASKLEDTEST_H
#define TASKLEDTEST_H

#include <Arduino.h>
#include "Global.h"
#include "HttpController.h"

#include "Sensors/SensorDefs.h"

#include <map>

#include "MQTTManager.h"
#include "Tasks/TaskBase.h"

class TaskLedTest : public TaskBase {

    public:
        TaskLedTest();

        void loop() override;
        
        void run() override;

        static constexpr int pinLedTest = 27;

        static void taskFunction(void* pvParameters);

    private:
        std::map<int, String> parametersValues;

        static void run2(void *parameter);

        struct TaskParams {
            TaskLedTest* obj;
        };

        void handleCallback(const String& topic, byte* payload, unsigned int length) override;
        void executeTask(TaskBase::TaskData taskData) override;

};
 #endif