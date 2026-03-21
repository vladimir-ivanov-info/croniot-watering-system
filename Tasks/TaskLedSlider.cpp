#include "TaskLedSlider.h"
#include <memory>

static const char* TAG = "TaskLedSlider";
static void analogWrite(int pin, int value);

TaskLedSlider::TaskLedSlider() : TaskBase("TaskLedSlider", 4096, 1, 1) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << pinLedTest);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(pinLedTest, 0); // Ensure the LED is off initially


 //analogWrite(pinLedTest, 190);


    ESP_LOGI(TAG, "TaskLedSlider initialized");
}

// PWM tipo Arduino: value 0..255 a 5 kHz en un pin
static void analogWrite(int pin, int value) {
    // 1) Timer (una sola vez)
    static bool timer_inited = false;
    if (!timer_inited) {
        ledc_timer_config_t t{};
        t.speed_mode      = LEDC_LOW_SPEED_MODE;     // C6: solo LOW_SPEED
        t.duty_resolution = LEDC_TIMER_8_BIT;        // 0..255
        t.timer_num       = LEDC_TIMER_0;
        t.freq_hz         = 5000;                    // 5 kHz
        t.clk_cfg         = LEDC_AUTO_CLK;
        // t.deconfigure   = false; // (si existe en tu IDF; si no, ignóralo)
        ESP_ERROR_CHECK(ledc_timer_config(&t));
        timer_inited = true;
    }

    // 2) Canal (puedes mapear pin→canal si vas a usar varios; aquí canal 0)
    ledc_channel_config_t ch{};
    ch.gpio_num   = pin;
    ch.speed_mode = LEDC_LOW_SPEED_MODE;
    ch.channel    = LEDC_CHANNEL_0;
    ch.intr_type  = LEDC_INTR_DISABLE;
    ch.timer_sel  = LEDC_TIMER_0;
    ch.duty       = 0;
    ch.hpoint     = 0;
    // ch.sleep_mode = LEDC_SLEEP_MODE_NONE; // si tu IDF lo tiene
    // ch.flags.output_invert = 0;           // si tu IDF lo tiene
    ESP_ERROR_CHECK(ledc_channel_config(&ch));

    // 3) Duty seguro (0..255) y tipo correcto
    value = std::clamp(value, 0, 255);
    uint32_t duty = static_cast<uint32_t>(value);

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}

void TaskLedSlider::loop() {
    SimpleTaskData *raw = nullptr;
    if (xQueueReceive(messageQueue, &raw, portMAX_DELAY) == pdPASS && raw) {
        std::unique_ptr<SimpleTaskData> taskData(raw);
        ESP_LOGI(TAG, "----------------> Received new task TaskLedSlider");
        executeTask(*taskData);
    }

   /* SimpleTaskData* taskDataPtr;
    if (xQueueReceive(messageQueue, &taskDataPtr, portMAX_DELAY) == pdPASS) {
        ESP_LOGI(TAG, "----------------> Received new task TaskLedSwitch");
        executeTask(*taskDataPtr);
        delete taskDataPtr;          // <-- IMPORTANTE
    }*/


    ESP_LOGI(TAG, "Loop finished");
}

void TaskLedSlider::run() {
    ESP_LOGI(TAG, "-------------------------------------------------- TaskLedSlider 1run() initialized.");

    std::string defaultState = "0";

    currentState = defaultState;
    TaskProgressUpdate taskProgressUpdate(getUid(), -1, defaultState, -1.0f, "");

    ESP_LOGI(TAG, "TaskLedSlider UID = %d enqueuing state %s", getUid(), currentState.c_str());

    TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);
    TaskController::instance().registerCallback(std::string(DEVICE_UUID), getUid(), this);
    //ESP_LOGI(TAG, "TaskLedSlider UID = %d", getUid());
}

void TaskLedSlider::executeTask(SimpleTaskData& taskData) {
    //TODO:
    //try {
        ESP_LOGI(TAG, "Running TaskLedSlider task");

        std::string parameterSliderState = taskData.parametersValues.at(TASK_LED_SLIDER_PARAMETER_SLIDER_STATE);

        int parameterSliderValue = atoi(parameterSliderState.c_str());



        ESP_LOGI(TAG, "RECEVIED VALUE FOR SLIDER: %d", parameterSliderValue);


        int pwmValue = 255*parameterSliderValue/100;
        ESP_LOGI(TAG, "PWM: %d", pwmValue);

        analogWrite(pinLedTest, pwmValue);

        //td::string texto = std::to_string(pwmValue);
        std::string texto = std::to_string(parameterSliderValue);
        currentState = texto;


        /*if (parameterSwitchState == "on") {
            gpio_set_level(pinLedTest, 1);
            currentState = "on";
        } else if (parameterSwitchState == "off") {
            gpio_set_level(pinLedTest, 0);
            currentState = "off";
        } else {
            ESP_LOGW(TAG, "Unknown switch state: %s", parameterSwitchState.c_str());
            currentState = "unknown";
        }*/

        TaskProgressUpdate taskProgressUpdate(getUid(), taskData.taskUid, parameterSliderState, 0.0, "");
                            TaskController::instance().enqueueTaskProgressUpdate(taskProgressUpdate);

    //} catch (const std::out_of_range& e) {
   //     ESP_LOGE(TAG, "Key not found: %d", TASK_LED_SWITCH_PARAMETER_SWITCH_STATE);
    //}
/**/
    ESP_LOGI(TAG, "TaskLedSlider task finished");
}
