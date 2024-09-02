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
       // TaskWaterPlants(std::map<int, String> parametersValues);
        //TaskLedTest(){};
        TaskLedTest(/*int taskUid, const std::map<int, String>& parametersValues*/);

        void loop() override;
        
        void run() override;

        static constexpr int pinLedTest = 27;

        //static constexpr int pinWaterPump = 4;
        //static constexpr int pinElectricValve1 = 5;
        //static constexpr int pinElectricValve2 = 18;

        static void taskFunction(void* pvParameters);

    private:

      //  int pinWaterPump = 4;
     //   int pinElectricValve1 = 5;
     //   int pinElectricValve2 = 18;
        //int taskUid;
        std::map<int, String> parametersValues;

        static void run2(void *parameter);

        struct TaskParams {
            TaskLedTest* obj;
        };

        //void registerCallbacks();
        //void handleCallback(String topic, byte* payload, unsigned int length);
        void handleCallback(const String& topic, byte* payload, unsigned int length) override;
        void executeTask(TaskBase::TaskData taskData) override;

        //DATA PASSING
        //QueueHandle_t messageQueue;

        /*struct Message {
            String topic;
            byte* payload;
            unsigned int length;
        };*/



};
 #endif