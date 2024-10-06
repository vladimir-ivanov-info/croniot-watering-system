#include "Tasks/TaskWaterPlants.h"

TaskWaterPlants::TaskWaterPlants(int taskUid, const std::map<int, String>& _parametersValues) : taskUid(taskUid), parametersValues(_parametersValues){
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
    Serial.print("TaskWaterPlants object address: ");
    Serial.println((unsigned long)this, HEX);

    //MQTTManager::instance().registerCallback("/server/esp32uuid_watering_system/task_type/" + String(TASK_WATER_PLANTS), this);


    Serial.print("Parameters size before:"); Serial.println(parametersValues.size());

    TaskParams* params = new TaskParams{this};
   // TaskParams* params = new TaskParams(this);
 //   HttpController().instance().sendHttpPost("", "/api/iot/task/");


    xTaskCreatePinnedToCore(
        TaskWaterPlants::run2,   // Task function
        "run2", // Task name
        4096,                 // Stack size (bytes)
        //8192,                 // Stack size (bytes)
        params,                 // Task parameters
        1,                    // Priority
        NULL,                 // Task handle
        0                     // Core to run the task (use 0 for the first core)
    );
    

  //TODO ensure voltage is initialized/read at least once or initialize to -1 in the beginning
  //if(Global::instance().voltage.toDouble() >= 12.00){
 /*   digitalWrite(pinElectricValve2, LOW);
    digitalWrite(pinElectricValve1, HIGH);
                                                                
    vTaskDelay(8000 / portTICK_PERIOD_MS);
    digitalWrite(pinWaterPump, HIGH);
    vTaskDelay(1000*60*8 / portTICK_PERIOD_MS); //8 mins
    digitalWrite(pinWaterPump, LOW);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    //DISCONNECT ELECTRIC VALVE
    digitalWrite(pinElectricValve2, HIGH);
    digitalWrite(pinElectricValve1, LOW);

    vTaskDelay(8000 / portTICK_PERIOD_MS);

    digitalWrite(pinElectricValve2, LOW);
    digitalWrite(pinElectricValve1, LOW); 
    */
  //} else {
 //   //ESP.deepSleep(60*60*60e6); //en microsegundos
  //}
}