#include "TaskLedSwitch.h"

TaskLedSwitch::TaskLedSwitch() : TaskBase("TaskLedSwitch", 4096, 1, 1) {
    pinMode(pinLedTest, OUTPUT);
    digitalWrite(pinLedTest, LOW); // Ensure the LED is off initially
}

//TODO this method can also be in the Base class
void TaskLedSwitch::loop() {
    SimpleTaskData* taskData;
    if (xQueueReceive(messageQueue, &taskData, portMAX_DELAY) == pdPASS) {
        executeTask(*taskData); //TODO maybe pass reference
    }
    Serial.println("Loop finished");
}

void TaskLedSwitch::run(){
    //DEFAULT BEHAVIOR IN CONSTRUCTOR: OFF
    TaskProgressUpdate taskProgressUpdate(getUid(), -1, "off", -1.0f, "");
    TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);

    TaskController::instance().registerCallback(static_cast<String>(DEVICE_UUID), getUid(), this);
}

void TaskLedSwitch::executeTask(SimpleTaskData& taskData){
    try {
        Serial.println("Running LedSwitch task");

        String parameterSwitchState = taskData.parametersValues.at(TASK_LED_SWITCH_PARAMETER_SWITCH_STATE);

        if(parameterSwitchState == "on"){
            digitalWrite(pinLedTest, HIGH);
        } else if(parameterSwitchState == "off") {
            digitalWrite(pinLedTest, LOW);
        } else {
            //TODO
        }
    
        TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, parameterSwitchState, 0.0, ""); //TODO RUNNING must be const later
        TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(static_cast<String>(TASK_LED_SWITCH_PARAMETER_SWITCH_STATE));
    }

    Serial.println("LedSwitch task finished");
}
