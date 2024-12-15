#include "TaskWaterPumpSwitch.h"

TaskWaterPumpSwitch::TaskWaterPumpSwitch() : TaskBase("TaskWaterPumpSwitch", 4096, 1, 1) {
    pinMode(pinWaterPump, OUTPUT);

    digitalWrite(pinWaterPump, HIGH);
}

//TODO this method can also be in the Base class
void TaskWaterPumpSwitch::loop() {
    SimpleTaskData* taskData;
    if (xQueueReceive(messageQueue, &taskData, portMAX_DELAY) == pdPASS) {
        executeTask(*taskData); //TODO maybe pass reference
    }
    Serial.println("Loop finished");
}

void TaskWaterPumpSwitch::run(){
    //DEFAULT BEHAVIOR IN CONSTRCUTOR: OFF
    TaskProgressUpdate taskProgressUpdate(getUid(), -1, "off", -1.0f, "");
    TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);

    TaskController::instance().registerCallback(static_cast<String>(DEVICE_UUID), getUid(), this);
}

void TaskWaterPumpSwitch::executeTask(SimpleTaskData& taskData){
    try {
        Serial.println("Running RelaySwitch task");

        String parameterSwitchState = taskData.parametersValues.at(TASK_RELAY_SWITCH_PARAMETER_SWITCH_STATE);

        if(parameterSwitchState == "on") {
            //digitalWrite(pinRelay, HIGH);
            digitalWrite(pinWaterPump, LOW);
        } else if(parameterSwitchState == "off") {
            //digitalWrite(pinRelay, LOW);
            digitalWrite(pinWaterPump, HIGH);
        } else {
            //TODO
        }

        TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, parameterSwitchState, 0.0, ""); //TODO RUNNING must be const later
        TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(static_cast<String>(TASK_WATER_PUMP_PARAMETER_SWITCH_STATE));
    }

    Serial.println("RelaySwitch task finished");
}
