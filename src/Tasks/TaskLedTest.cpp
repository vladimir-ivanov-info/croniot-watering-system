#include "TaskLedTest.h"

TaskLedTest::TaskLedTest()
    : TaskBase("TaskLedTest", 4096, 1, 1) {
    pinMode(pinLedTest, OUTPUT);
    digitalWrite(pinLedTest, LOW); // Ensure the LED is off initially
}

//TODO this method can also be in the Base class
void TaskLedTest::loop() {

    Message message;
    if (xQueueReceive(messageQueue, &message, portMAX_DELAY) == pdPASS) {
        Serial.println("Received message from queue: ");
        String payloadString = byteArrayToString(message.payload, message.length);
        Serial.println(payloadString);

        TaskBase::TaskData taskData = processMessage(payloadString);

        int taskUid = taskData.taskUid;

        String mqttStateTopic = String("/api/iot/") + "esp32uuid" + "/task_type/" + String(TASK_LED_TEST) + "/" + String(taskUid) + "/state/";

        HttpController().instance().sendHttpPost("RECEIVED", mqttStateTopic);

        //Serial.println("Before receeiving:");
        //Serial.println(taskData.parametersValues.size());

        executeTask(taskData); //TODO maybe pass reference
        HttpController().instance().sendHttpPost("COMPLETED", mqttStateTopic);
        vPortFree(message.payload); // Free the payload after processing
        // vTaskDelay(pdMS_TO_TICKS(5000));
    }
    Serial.println("Loop finished");
}

void TaskLedTest::taskFunction(void* pvParameters) {

    TaskParams* params = static_cast<TaskParams*>(pvParameters);

    while (true) {
        TaskBase::Message message;
        if (params->obj->messageQueue == NULL) {
            Serial.println("Message queue is NULL.");
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay to avoid flooding the serial output
            continue;
        }
       vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL); // Delete the task after registering callbacks
}

/*
void TaskLedTest::registerCallbacks() {
    MQTTManager::instance().registerCallback("/server/esp32uuid/task_type/" + String(TASK_LED_TEST), [this](String topic, byte* payload, unsigned int length) {
        this->handleCallback(topic, payload, length);
    });
}*/
/*
void TaskLedTest::registerCallbacks() {
    MQTTManager::instance().registerCallback("/server/esp32uuid/task_type/" + String(TASK_LED_TEST), this);
}*/

/*
void TaskLedTest::handleCallback(String topic, byte* payload, unsigned int length) {
    Serial.println("Handling task for LED test!");
    // Handle the task here
    // Deserialize JSON, etc.
    run2();
}*/

void TaskLedTest::handleCallback(const String& topic, byte* payload, unsigned int length) {
    Serial.println("Handling task for LED TEST");
    // Handle the task here
    // Deserialize JSON, etc.
    // Process the payload and take necessary actions
    //run2();
}
/*
void TaskLedTest::run() {
    // Task execution code
}*/

void TaskLedTest::run2(void *parameter){

    if (parameter == nullptr) {
        Serial.println("Error: parameter is null");
        vTaskDelete(NULL);
        return;
    }

    TaskParams* params = static_cast<TaskParams*>(parameter);
    //Serial.println(">>>Runnning task casted succesfully...");
    TaskLedTest* task = params->obj;

    //Serial.println(">>>Runnning task task obj accessed succesfully...");
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

        digitalWrite(task->pinLedTest, HIGH);

       // Serial.println(">>>Runnning task pin configured succesfully...");       
        Serial.print("Running LedTest task for "); Serial.print(durationSeconds); Serial.println(" seconds");
        //vTaskDelay(1000*60*8 / portTICK_PERIOD_MS); //8 mins
        vTaskDelay(1000 * durationSeconds / portTICK_PERIOD_MS);
        Serial.print("LedTest stopped ");
        digitalWrite(task->pinLedTest, LOW);
        //HttpController().instance().sendHttpPost("RECEIVED", "/api/iot/" + deviceUuid  + "/task_type/" + String(taskTypeUid) + "/" + String(taskUid) + "/state/");

    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(parameterDuration);
    }

    Serial.println("LedTest task finished");
    vTaskDelete(NULL);

    //void MQTTManager::publish(String topic, String message){
}



void TaskLedTest::run(){
    MQTTManager::instance().registerCallback("/server/esp32uuid_watering_system/task_type/" + String(TASK_LED_TEST), this);


    /*TaskParams* params = new TaskParams{this};

    xTaskCreatePinnedToCore(
            TaskLedTest::taskFunction,       // Function to implement the task
            "TaskLedTest",         // Name of the task
            4096,               // Stack size in words
            params,               // Task input parameter
            1,                  // Priority of the task
            NULL,               // Task handle
            1                   // Core where the task should run
        );*/

   // Serial.print("TaskWaterPlants object address: ");
   // Serial.println((unsigned long)this, HEX);

  //  Serial.print("Parameters size before:"); Serial.println(parametersValues.size());

  //  TaskParams* params = new TaskParams{this};
   // TaskParams* params = new TaskParams(this);
 //   HttpController().instance().sendHttpPost("", "/api/iot/task/");


    /*xTaskCreatePinnedToCore(
        TaskLedTest::run2,   // Task function
        "run2", // Task name
        4096,                 // Stack size (bytes)
        //8192,                 // Stack size (bytes)
        params,                 // Task parameters
        1,                    // Priority
        NULL,                 // Task handle
        0                     // Core to run the task (use 0 for the first core)
    );*/

   /* xTaskCreatePinnedToCore(
        TaskLedTest::taskFunction,   // Task function
        "RegisterCallbacks", // Task name
        4096,                 // Stack size (bytes)
        //8192,                 // Stack size (bytes)
        params,                 // Task parameters
        1,                    // Priority
        NULL,                 // Task handle
        0                     // Core to run the task (use 0 for the first core)
    );*/


      //  MQTTManager::instance().registerCallback("/server/esp32uuid/task_type/1", waterPlantsTask);

   // registerCa

}

void TaskLedTest::executeTask(TaskBase::TaskData taskData){
    int parameterDuration = static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION); //TODO not water plants, but LED
    try {
        Serial.print("Parameters size:"); Serial.println(taskData.parametersValues.size());
        Serial.println(parameterDuration);
        String parameterDurationValue = taskData.parametersValues.at(parameterDuration);
        int durationSeconds = parameterDurationValue.toInt();

        digitalWrite(pinLedTest, HIGH);

       // Serial.println(">>>Runnning task pin configured succesfully...");       
        Serial.print("Running LedTest task for "); Serial.print(durationSeconds); Serial.println(" seconds");
        //vTaskDelay(1000*60*8 / portTICK_PERIOD_MS); //8 mins
        vTaskDelay(1000 * durationSeconds / portTICK_PERIOD_MS);
        Serial.print("LedTest stopped ");
        digitalWrite(pinLedTest, LOW);
        //HttpController().instance().sendHttpPost("RECEIVED", "/api/iot/" + deviceUuid  + "/task_type/" + String(taskTypeUid) + "/" + String(taskUid) + "/state/");

    } catch (const std::out_of_range& e) {
        Serial.print("Key not found: ");
        Serial.println(parameterDuration);
    }

    Serial.println("LedTest task finished");
}
