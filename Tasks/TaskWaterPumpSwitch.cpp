#include "TaskWaterPumpSwitch.h"
#include <memory>

static const char* TAG = "TaskWaterPumpSwitch";

TaskWaterPumpSwitch::TaskWaterPumpSwitch() : TaskBase("TaskWaterPumpSwitch", 4096, 1, 1) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << pinWaterPump);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(pinWaterPump, 1);  // Initial state: OFF (HIGH = OFF for active-low relay)
}

void TaskWaterPumpSwitch::loop() {
    SimpleTaskData* raw = nullptr;
    if (xQueueReceive(messageQueue, &raw, portMAX_DELAY) == pdPASS && raw) {
        std::unique_ptr<SimpleTaskData> taskData(raw);
        executeTask(*taskData);
    }
    ESP_LOGI(TAG, "Loop finished");
}

void TaskWaterPumpSwitch::run() {
    TaskProgressUpdate update(getUid(), -1, "off", -1.0f, "");
    TaskController::instance().enqueueTaskProgressUpdate(update);

    TaskController::instance().registerCallback(std::string(DEVICE_UUID), getUid(), this);
}

void TaskWaterPumpSwitch::executeTask(SimpleTaskData& taskData) {
    //TODO:
    //try {
        ESP_LOGI(TAG, "Running RelaySwitch task");

        std::string state = taskData.parametersValues.at(TASK_RELAY_SWITCH_PARAMETER_SWITCH_STATE);

        if (state == "on") {
            gpio_set_level(pinWaterPump, 0);  // Active LOW
        } else if (state == "off") {
            gpio_set_level(pinWaterPump, 1);
        } else {
            ESP_LOGW(TAG, "Unknown state: %s", state.c_str());
        }

        TaskProgressUpdate update(getUid(), taskData.taskUid, state, 0.0, "");
        TaskController::instance().enqueueTaskProgressUpdate(update);
    //} catch (const std::out_of_range&) {
    //    ESP_LOGE(TAG, "Key not found: %d", TASK_WATER_PUMP_PARAMETER_SWITCH_STATE);
    //}

    ESP_LOGI(TAG, "RelaySwitch task finished");
}
