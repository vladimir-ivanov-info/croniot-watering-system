#include "Tasks/TaskWaterPlants.h"

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

    SimpleTaskData* taskData;
    if (xQueueReceive(messageQueue, &taskData, portMAX_DELAY) == pdPASS) {
        executeTask(*taskData); //TODO maybe pass reference
    }
    Serial.println("Loop finished");
}

void TaskWaterPlants::run(){
   TaskController::instance().registerCallback(static_cast<String>(DEVICE_UUID), getUid(), this);
}

void TaskWaterPlants::executeTask(SimpleTaskData& taskData){

    TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", 0.0, ""); //TODO RUNNING must be enum later
    TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);


    Serial.println("--->Running watering task!");
    int parameterDuration = static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION); //TODO not water plants, but LED

    try {
        Serial.print("Parameters size:"); Serial.println(taskData.parametersValues.size());
        Serial.println(parameterDuration);
        String parameterDurationValue = taskData.parametersValues.at(parameterDuration);
        
        int wateringDurationMillis = parameterDurationValue.toInt() * 1000;
        int waterValveOpeningMillis = 8000;
        int waterValveClosingMillis = 8000;
        int closingValveDelayMillis = 1000; 
        int totalProcessDurationSeconds = waterValveOpeningMillis + wateringDurationMillis + waterValveClosingMillis + closingValveDelayMillis;

        int elapsedMillis = 0;

        openWaterValve();

        //vTaskDelay(waterValveOpeningMillis / portTICK_PERIOD_MS);

        for(int i = 1000; i <  waterValveOpeningMillis + 1000; i+=1000){
            
            float progressPercentage = (float)elapsedMillis*100.0f/(float)totalProcessDurationSeconds;
            progressPercentage = std::round(progressPercentage * 100) / 100;

            TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", progressPercentage, ""); 
            TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
            vTaskDelay(1*1000 / portTICK_PERIOD_MS);

            elapsedMillis+=1000;
        }

        startWatering();

        //vTaskDelay(wateringDurationMillis / portTICK_PERIOD_MS);

        for(int i = 1000; i <  wateringDurationMillis + 1000; i+=1000){
            
            float progressPercentage = (float)elapsedMillis*100.0f/(float)totalProcessDurationSeconds;
            progressPercentage = std::round(progressPercentage * 100) / 100;

            TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", progressPercentage, ""); 
            TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
            vTaskDelay(1*1000 / portTICK_PERIOD_MS);

            elapsedMillis+=1000;
        }


        stopWatering();
        //vTaskDelay(closingValveDelayMillis / portTICK_PERIOD_MS);
        //1 second delay between water pump turn off and water valve closing
        for(int i = 1000; i <  closingValveDelayMillis + 1000; i+=1000){
            
            float progressPercentage = (float)elapsedMillis*100.0f/(float)totalProcessDurationSeconds;
            progressPercentage = std::round(progressPercentage * 100) / 100;

            TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", progressPercentage, ""); 
            TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
            vTaskDelay(1*1000 / portTICK_PERIOD_MS);

            elapsedMillis+=1000;
        }


        closeWaterValve();
        //vTaskDelay(closingValveDelayMillis / portTICK_PERIOD_MS); //close valve
        for(int i = 0; i <  waterValveClosingMillis + 1000; i+=1000){
            
            float progressPercentage = (float)elapsedMillis*100.0f/(float)totalProcessDurationSeconds;
            progressPercentage = std::round(progressPercentage * 100) / 100;

            TaskProgressUpdate taskProgressUpdate;
            if(progressPercentage < 100.0f){
                taskProgressUpdate = TaskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", progressPercentage, "");
            } else {
                taskProgressUpdate = TaskProgressUpdate(getUid(), taskData.taskUid, "COMPLETED", 100.0f, ""); 
            }

            TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
            vTaskDelay(1*1000 / portTICK_PERIOD_MS);

            elapsedMillis+=1000;
        }
    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(parameterDuration);
    }

    Serial.println("Watering task finished");
}


void TaskWaterPlants::openWaterValve(){
    digitalWrite(TaskWaterPlants::pinElectricValve1, HIGH);
    digitalWrite(TaskWaterPlants::pinElectricValve2, LOW);
}

void TaskWaterPlants::closeWaterValve(){
    digitalWrite(TaskWaterPlants::pinElectricValve2, HIGH);
    digitalWrite(TaskWaterPlants::pinElectricValve1, LOW);
}

void TaskWaterPlants::startWatering(){
    digitalWrite(TaskWaterPlants::pinWaterPump, TaskWaterPlants::relayOnState);
}

void TaskWaterPlants::stopWatering(){
    digitalWrite(TaskWaterPlants::pinWaterPump, TaskWaterPlants::relayOffState);
}