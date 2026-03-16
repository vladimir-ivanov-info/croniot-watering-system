#ifndef TASKWATERPUMPSWITCH_H
#define TASKWATERPUMPSWITCH_H

#include <map>
#include <string>

#include "driver/gpio.h"
#include "esp_log.h"

#include "Tasks/TaskBase.h"
#include "Tasks/TaskData.h"
#include "Tasks/SimpleTaskData.h"
#include "Tasks/TaskProgressUpdate.h"
#include "Tasks/TaskController.h"

#include "Sensors/SensorDefs.h"
#include "secrets.h"

class TaskWaterPumpSwitch : public TaskBase {

public:
    TaskWaterPumpSwitch();
    int getUid() override { return TASK_WATER_PUMP_SWITCH; }

    void loop() override;
    void run() override;

    static constexpr gpio_num_t pinWaterPump = GPIO_NUM_4;

private:
    std::map<int, std::string> parametersValues;

    struct TaskParams {
        TaskWaterPumpSwitch* obj;
    };

    void executeTask(SimpleTaskData& taskData) override;
};

#endif
