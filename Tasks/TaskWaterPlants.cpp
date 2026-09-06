#include "TaskWaterPlants.h"
#include "Tasks/TaskWaterPlantsLogic.h"
#include <memory>

static const char* TAG = "TaskWaterPlants";

TaskWaterPlants::TaskWaterPlants()
    : TaskBase("TaskWaterPlants", 4096, 1, 1),
      gpioDriver_(pinWaterPump, pinElectricValve1, pinElectricValve2),
      sequence_(gpioDriver_) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << pinWaterPump) | (1ULL << pinElectricValve1) | (1ULL << pinElectricValve2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    sequence_.driveToSafeState();
}

void TaskWaterPlants::loop() {
    SimpleTaskData* raw = nullptr;
    if (xQueueReceive(messageQueue, &raw, portMAX_DELAY) == pdPASS && raw) {
        std::unique_ptr<SimpleTaskData> taskData(raw);
        executeTask(*taskData);
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

    auto it = taskData.parametersValues.find(parameterDuration);
    if (it == taskData.parametersValues.end()) {
        ESP_LOGE(TAG, "Key not found: %d", parameterDuration);
        return;
    }

    auto parsedSeconds = TaskWaterPlantsLogic::parseDurationSeconds(it->second);
    if (!parsedSeconds.has_value()) {
        ESP_LOGE(TAG, "Invalid duration value: %s", it->second.c_str());
        return;
    }

    int durationSeconds = TaskWaterPlantsLogic::clampDurationSeconds(*parsedSeconds);
    int wateringDurationMillis = durationSeconds * 1000;
    int totalDuration = TaskWaterPlantsLogic::computeTotalDurationMillis(wateringDurationMillis);
    int elapsed = 0;

    auto sendProgress = [&](int elapsedMillis) {
        float progress = TaskWaterPlantsLogic::computeProgress(elapsedMillis, totalDuration);
        TaskProgressUpdate prog(getUid(), taskData.taskUid, "RUNNING", progress, "");
        TaskController::instance().enqueueTaskProgressUpdate(prog);
    };

    sequence_.openWaterValve();
    for (int i = 0; i < TaskWaterPlantsLogic::kOpenTimeMillis; i += 1000) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        elapsed += 1000;
        sendProgress(elapsed);
    }

    sequence_.startWatering();
    for (int i = 0; i < wateringDurationMillis; i += 1000) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        elapsed += 1000;
        sendProgress(elapsed);
    }

    sequence_.stopWatering();
    for (int i = 0; i < TaskWaterPlantsLogic::kCloseDelayMillis; i += 1000) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        elapsed += 1000;
        sendProgress(elapsed);
    }

    sequence_.closeWaterValve();
    for (int i = 0; i < TaskWaterPlantsLogic::kCloseTimeMillis; i += 1000) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        elapsed += 1000;
        float progress = TaskWaterPlantsLogic::computeProgress(elapsed, totalDuration);

        TaskProgressUpdate prog;
        if (progress < 100.0f) {
            prog = TaskProgressUpdate(getUid(), taskData.taskUid, "RUNNING", progress, "");
        } else {
            prog = TaskProgressUpdate(getUid(), taskData.taskUid, "COMPLETED", 100.0f, "");
        }
        TaskController::instance().enqueueTaskProgressUpdate(prog);
    }

    ESP_LOGI(TAG, "Watering task finished");
}
