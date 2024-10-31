#include "TaskLedTest.h"

TaskLedTest::TaskLedTest() : TaskBase("TaskLedTest", 4096, 1, 1) {
    pinMode(pinLedTest, OUTPUT);
    digitalWrite(pinLedTest, LOW); // Ensure the LED is off initially
}

//TODO this method can also be in the Base class
void TaskLedTest::loop() {
    SimpleTaskData* taskData;
    if (xQueueReceive(messageQueue, &taskData, portMAX_DELAY) == pdPASS) {
        executeTask(*taskData); //TODO maybe pass reference
    }
    Serial.println("Loop finished");
}

void TaskLedTest::run(){
    TaskController::instance().registerCallback(static_cast<String>(DEVICE_UUID), static_cast<int>(TASK_LED_TEST), this);
}

void TaskLedTest::executeTask(SimpleTaskData& taskData){

    TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", 0.0, ""); //TODO RUNNING must be enum later
    TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);

    int parameterDuration = static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION); //TODO not water plants, but LED
    try {
        Serial.print("Parameters size:"); Serial.println(taskData.parametersValues.size());
        Serial.println(parameterDuration);
        String parameterDurationValue = taskData.parametersValues.at(parameterDuration);
        int durationSeconds = parameterDurationValue.toInt();

        digitalWrite(pinLedTest, HIGH);

        Serial.print("Running LedTest task for "); Serial.print(durationSeconds); Serial.println(" seconds");
        
        for(int i = 1; i < durationSeconds + 1; i++){

            float progressPercentage = i*100/durationSeconds;

            TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", progressPercentage, ""); 
            TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
            vTaskDelay(1*1000 / portTICK_PERIOD_MS);
        }

        Serial.print("LedTest stopped ");
        digitalWrite(pinLedTest, LOW);

        TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "COMPLETED", 100.0, ""); 
        TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(parameterDuration);
    }

    Serial.println("LedTest task finished");
}
