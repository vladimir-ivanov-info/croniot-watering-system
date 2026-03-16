#ifndef TASKLEDSLIDER_H
#define TASKLEDSLIDER_H

#include <map>
#include <string>

#include "Tasks/TaskBase.h"
#include "Tasks/TaskData.h"
#include "Tasks/SimpleTaskData.h"
#include "Tasks/TaskProgressUpdate.h"
#include "Tasks/TaskController.h"

#include "Sensors/SensorDefs.h"
#include "secrets.h"

#include "driver/gpio.h"
#include "esp_log.h"

//#include "driver/ledc.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include <algorithm> // std::clamp

class TaskLedSlider : public TaskBase {

public:
    TaskLedSlider();
    int getUid() override { return TASK_LED_SLIDER; }

    void loop() override;
    void run() override;

    static constexpr gpio_num_t pinLedTest = GPIO_NUM_2;

private:
    std::map<int, std::string> parametersValues;
    void executeTask(SimpleTaskData& taskData) override;
};

#endif
