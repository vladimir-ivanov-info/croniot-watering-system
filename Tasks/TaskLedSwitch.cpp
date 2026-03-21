#include "TaskLedSwitch.h"
#include "esp_timer.h"
#include <memory>

static const char* TAG = "TaskLedSwitch";

TaskLedSwitch::TaskLedSwitch() : TaskBase("TaskLedSwitch", 4096, 1, 1) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << pinLedTest);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(pinLedTest, 0); // Ensure the LED is off initially

    ESP_LOGI(TAG, "TaskLedSwitch initialized");
}

void TaskLedSwitch::loop() {
    SimpleTaskData *raw = nullptr;
    if (xQueueReceive(messageQueue, &raw, portMAX_DELAY) == pdPASS && raw) {
        std::unique_ptr<SimpleTaskData> taskData(raw);
        int64_t tReceived = esp_timer_get_time();
        ESP_LOGW(TAG, "TIMING: task dequeued at %lld", tReceived);
        executeTask(*taskData);
        int64_t tDone = esp_timer_get_time();
        ESP_LOGW(TAG, "TIMING: executeTask took %lldus", (tDone - tReceived));
    }

   /* SimpleTaskData* taskDataPtr;
    if (xQueueReceive(messageQueue, &taskDataPtr, portMAX_DELAY) == pdPASS) {
        ESP_LOGI(TAG, "----------------> Received new task TaskLedSwitch");
        executeTask(*taskDataPtr);
        delete taskDataPtr;          // <-- IMPORTANTE
    }*/


    ESP_LOGI(TAG, "Loop finished");
}

void TaskLedSwitch::run() {
    ESP_LOGI(TAG, "-------------------------------------------------- TaskLedSwitch 1run() initialized.");
    currentState = "off";
    TaskProgressUpdate taskProgressUpdate(getUid(), -1, "off", -1.0f, "");

    ESP_LOGI(TAG, "TaskLedSwitch UID = %d enqueuing state %s", getUid(), "off");

    TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
    TaskController::instance().registerCallback(std::string(DEVICE_UUID), getUid(), this);
    //ESP_LOGI(TAG, "TaskLedSwitch UID = %d", getUid());
}

void TaskLedSwitch::executeTask(SimpleTaskData& taskData) {
    //TODO:
    //try {
        ESP_LOGI(TAG, "Running LedSwitch task");

        std::string parameterSwitchState = taskData.parametersValues.at(TASK_LED_SWITCH_PARAMETER_SWITCH_STATE);

        if (parameterSwitchState == "on") {
            gpio_set_level(pinLedTest, 1);
            currentState = "on";
        } else if (parameterSwitchState == "off") {
            gpio_set_level(pinLedTest, 0);
            currentState = "off";
        } else {
            ESP_LOGW(TAG, "Unknown switch state: %s", parameterSwitchState.c_str());
            currentState = "unknown";
        }

        TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, parameterSwitchState, 0.0, "");
                            TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);

    //} catch (const std::out_of_range& e) {
   //     ESP_LOGE(TAG, "Key not found: %d", TASK_LED_SWITCH_PARAMETER_SWITCH_STATE);
    //}
/**/
    ESP_LOGI(TAG, "LedSwitch task finished");
}
