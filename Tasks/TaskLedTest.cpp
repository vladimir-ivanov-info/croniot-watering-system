#include "TaskLedTest.h"
#include <memory>

static const char* TAG = "TaskLedTest";

TaskLedTest::TaskLedTest() : TaskBase("TaskLedTest", 4096, 1, 1) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << pinLedTest);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(pinLedTest, 0); // Start with LED off
}

void TaskLedTest::loop() {
    SimpleTaskData* raw = nullptr;
    if (xQueueReceive(messageQueue, &raw, portMAX_DELAY) == pdPASS && raw) {
        std::unique_ptr<SimpleTaskData> taskData(raw);
        executeTask(*taskData);
    }
    ESP_LOGI(TAG, "Loop finished");
}

void TaskLedTest::run() {
     ESP_LOGI(TAG, "REGISTERING CALLBACK");
    TaskController::instance().registerCallback(std::string(DEVICE_UUID), getUid(), this);
     ESP_LOGI(TAG, "CALLBACK REGOISTERED");
}

void TaskLedTest::executeTask(SimpleTaskData& taskData) {
    TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", 0.0, "");
    TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);

    int parameterDuration = static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION); // TODO: rename constant

    //try {
        ESP_LOGI(TAG, "Parameters size: %d", (int)taskData.parametersValues.size());

        std::string durationStr = taskData.parametersValues.at(parameterDuration);
        int durationSeconds = std::stoi(durationStr);

        gpio_set_level(pinLedTest, 1);
        ESP_LOGI(TAG, "Running LedTest for %d seconds", durationSeconds);

        for (int i = 1; i <= durationSeconds; i++) {
            float progressPercentage = i * 100.0f / durationSeconds;

            TaskProgressUpdate progress(getUid(), taskData.taskUid, "RUNNING", progressPercentage, "");
            TaskController::instance().enqueueTaskProgressUpdate(progress);

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        gpio_set_level(pinLedTest, 0);
        ESP_LOGI(TAG, "LedTest finished, LED turned off");

        TaskProgressUpdate completed(getUid(), taskData.taskUid, "COMPLETED", 100.0f, "");
        TaskController::instance().enqueueTaskProgressUpdate(completed);
//TODO:
    //} catch (const std::out_of_range& e) {
    //    ESP_LOGE(TAG, "Key not found: %d", parameterDuration);
    //}
}
