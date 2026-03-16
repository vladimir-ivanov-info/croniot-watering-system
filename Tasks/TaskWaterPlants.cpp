#include "TaskWaterPlants.h"
#include <cmath>

static const char* TAG = "TaskWaterPlants";

TaskWaterPlants::TaskWaterPlants() : TaskBase("TaskWaterPlants", 4096, 1, 1) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << pinWaterPump) | (1ULL << pinElectricValve1) | (1ULL << pinElectricValve2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(pinElectricValve2, 1);
    gpio_set_level(pinElectricValve1, 0);
    gpio_set_level(pinWaterPump, relayOffState);

    // DISCONNECT
    gpio_set_level(pinElectricValve2, 0);
    gpio_set_level(pinElectricValve1, 0);
}

void TaskWaterPlants::loop() {
    SimpleTaskData* taskData = nullptr;
    if (xQueueReceive(messageQueue, &taskData, portMAX_DELAY) == pdPASS && taskData) {
        executeTask(*taskData);
        delete taskData;
    }
    ESP_LOGI(TAG, "Loop finished");
}

void TaskWaterPlants::run() {
    TaskController::instance().registerCallback(std::string(DEVICE_UUID), getUid(), this);
}

void TaskWaterPlants::executeTask(SimpleTaskData& taskData) {
    TaskProgressUpdate startProgress(getUid(), taskData.taskUid, "RUNNING", 0.0f, "");
    TaskController::instance().enqueueTaskProgressUpdate(startProgress);

    ESP_LOGI(TAG, "Running watering task");
    int parameterDuration = static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION);

    //TODO:
    //try {
        std::string durationStr = taskData.parametersValues.at(parameterDuration);
        int wateringDurationMillis = std::stoi(durationStr) * 1000;

        const int openTime = 8000;
        const int closeDelay = 1000;
        const int closeTime = 8000;

        const int totalDuration = openTime + wateringDurationMillis + closeDelay + closeTime;
        int elapsed = 0;

        auto sendProgress = [&](int elapsedMillis) {
            float progress = static_cast<float>(elapsedMillis) * 100.0f / totalDuration;
            progress = std::round(progress * 100) / 100;
            TaskProgressUpdate prog(getUid(), taskData.taskUid, "RUNNING", progress, "");
            TaskController::instance().enqueueTaskProgressUpdate(prog);
        };

        openWaterValve();
        for (int i = 0; i < openTime; i += 1000) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            elapsed += 1000;
            sendProgress(elapsed);
        }

        startWatering();
        for (int i = 0; i < wateringDurationMillis; i += 1000) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            elapsed += 1000;
            sendProgress(elapsed);
        }

        stopWatering();
        for (int i = 0; i < closeDelay; i += 1000) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            elapsed += 1000;
            sendProgress(elapsed);
        }

        closeWaterValve();
        for (int i = 0; i < closeTime; i += 1000) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            elapsed += 1000;
            float progress = static_cast<float>(elapsed) * 100.0f / totalDuration;
            progress = std::round(progress * 100) / 100;

            TaskProgressUpdate prog;
            if (progress < 100.0f) {
                prog = TaskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", progress, "");
            } else {
                prog = TaskProgressUpdate(getUid(), taskData.taskUid, "COMPLETED", 100.0f, "");
            }
            TaskController::instance().enqueueTaskProgressUpdate(prog);
        }

    //} catch (const std::out_of_range&) {
    //    ESP_LOGE(TAG, "Key not found: %d", parameterDuration);
    //}

    ESP_LOGI(TAG, "Watering task finished");
}

void TaskWaterPlants::openWaterValve() {
    gpio_set_level(pinElectricValve1, 1);
    gpio_set_level(pinElectricValve2, 0);
}

void TaskWaterPlants::closeWaterValve() {
    gpio_set_level(pinElectricValve2, 1);
    gpio_set_level(pinElectricValve1, 0);
}

void TaskWaterPlants::startWatering() {
    gpio_set_level(pinWaterPump, relayOnState);
}

void TaskWaterPlants::stopWatering() {
    gpio_set_level(pinWaterPump, relayOffState);
}
