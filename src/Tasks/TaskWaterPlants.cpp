#include "Tasks/TaskWaterPlants.h"

//TaskWaterPlants::TaskWaterPlants(int taskUid, const std::map<int, String>& _parametersValues) : taskUid(taskUid), parametersValues(_parametersValues){

TaskWaterPlants::TaskWaterPlants() : TaskBase("TaskWaterPlants", 4096, 1, 1){
    pinMode(pinWaterPump, OUTPUT);
    pinMode(pinElectricValve1, OUTPUT);
    pinMode(pinElectricValve2, OUTPUT);

    digitalWrite(pinElectricValve2, HIGH);
    digitalWrite(pinElectricValve1, LOW);

    digitalWrite(pinWaterPump, relayOffState);

    //DISCONNECT
    digitalWrite(pinElectricValve2, LOW);
    digitalWrite(pinElectricValve1, LOW);
}

void TaskWaterPlants::loop() {

    Message message;
    if (xQueueReceive(messageQueue, &message, portMAX_DELAY) == pdPASS) {
        Serial.println("Received message from queue: ");
        String payloadString = byteArrayToString(message.payload, message.length);
        Serial.println(payloadString);

        TaskBase::TaskData taskData = processMessage(payloadString);

        int taskUid = taskData.taskUid;

        String mqttStateTopic = String("/api/iot/") + static_cast<String>(DEVICE_UUID) + "/task_type/" + String(TASK_WATER_PLANTS) + "/" + String(taskUid) + "/state/";
        HttpController().instance().sendHttpPost("RECEIVED", mqttStateTopic);
        executeTask(taskData); //TODO maybe pass reference
        HttpController().instance().sendHttpPost("COMPLETED", mqttStateTopic);
        vPortFree(message.payload); // Free the payload after processing
    }
    Serial.println("Loop finished");
}


void TaskWaterPlants::run2(void *parameter){

    if (parameter == nullptr) {
        Serial.println("Error: parameter is null");
        vTaskDelete(NULL);
        return;
    }

    TaskParams* params = static_cast<TaskParams*>(parameter);
    Serial.println(">>>Runnning task casted succesfully...");
    TaskWaterPlants* task = params->obj;
    
    Serial.print("Received TaskWaterPlants object address: ");
    Serial.println((unsigned long)task, HEX);

    Serial.print("pinWaterPump: ");
    Serial.println(task->pinWaterPump);
    Serial.print("pinElectricValve1: ");
    Serial.println(task->pinElectricValve1);
    Serial.print("pinElectricValve2: ");
    Serial.println(task->pinElectricValve2);

    Serial.println(">>>Runnning task task obj accessed succesfully...");
    if (task == nullptr) {
        Serial.println("Error: task is null");
        vTaskDelete(NULL);
        return;
    }
    
    int parameterDuration = static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION);
    try {
        Serial.print("Parameters size:"); Serial.println(task->parametersValues.size());
        Serial.println(parameterDuration);
        String parameterDurationValue = task->parametersValues.at(parameterDuration);
        int durationSeconds = parameterDurationValue.toInt();

        digitalWrite(task->pinElectricValve1, HIGH);
        digitalWrite(task->pinElectricValve2, LOW);
       
        Serial.println(">>>Runnning task pin configured succesfully...");       
        vTaskDelay(8000 / portTICK_PERIOD_MS);
        digitalWrite(task->pinWaterPump, task->relayOnState);
        Serial.print("Running Watering task for "); Serial.print(durationSeconds); Serial.println(" seconds");
        vTaskDelay(1000 * durationSeconds / portTICK_PERIOD_MS);
        Serial.print("Watering stopped ");
        digitalWrite(task->pinWaterPump, task->relayOffState);

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        //DISCONNECT ELECTRIC VALVE
        digitalWrite(task->pinElectricValve2, HIGH);
        digitalWrite(task->pinElectricValve1, LOW);

        vTaskDelay(8*1000 / portTICK_PERIOD_MS); //close valve

        digitalWrite(task->pinElectricValve2, LOW);
        digitalWrite(task->pinElectricValve1, LOW); 
    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(parameterDuration);
    }

    Serial.println("Watering task finished");
    vTaskDelete(NULL);
}

void TaskWaterPlants::run(){
    MQTTManager::instance().registerCallback("/server/" + static_cast<String>(DEVICE_UUID) + "/task_type/" + String(TASK_WATER_PLANTS), this);
}

void TaskWaterPlants::executeTask(TaskBase::TaskData taskData){
    int parameterDuration = static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION); //TODO not water plants, but LED

    try {
        Serial.print("Parameters size:"); Serial.println(taskData.parametersValues.size());
        Serial.println(parameterDuration);
        String parameterDurationValue = taskData.parametersValues.at(parameterDuration);
        int durationSeconds = parameterDurationValue.toInt();

        digitalWrite(TaskWaterPlants::pinElectricValve1, HIGH);
        digitalWrite(TaskWaterPlants::pinElectricValve2, LOW);
       
        Serial.println(">>>Runnning task pin configured succesfully...");       
        vTaskDelay(8000 / portTICK_PERIOD_MS);
        digitalWrite(TaskWaterPlants::pinWaterPump, TaskWaterPlants::relayOnState);
        Serial.print("Running Watering task for "); Serial.print(durationSeconds); Serial.println(" seconds");
        vTaskDelay(1000 * durationSeconds / portTICK_PERIOD_MS);
        Serial.print("Watering stopped ");
        digitalWrite(TaskWaterPlants::pinWaterPump, TaskWaterPlants::relayOffState);

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        //DISCONNECT ELECTRIC VALVE
        digitalWrite(TaskWaterPlants::pinElectricValve2, HIGH);
        digitalWrite(TaskWaterPlants::pinElectricValve1, LOW);

        vTaskDelay(8*1000 / portTICK_PERIOD_MS); //close valve

        digitalWrite(TaskWaterPlants::pinElectricValve2, LOW);
        digitalWrite(TaskWaterPlants::pinElectricValve1, LOW); 
    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(parameterDuration);
    }

    Serial.println("Watering task finished");
}

void TaskWaterPlants::handleCallback(const String& topic, byte* payload, unsigned int length) {
    Serial.println("Handling task for LED TEST");
    // Handle the task here
    // Deserialize JSON, etc.
    // Process the payload and take necessary actions
    //run2();
}