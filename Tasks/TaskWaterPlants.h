#ifndef TASKWATERPLANTS_H
#define TASKWATERPLANTS_H

#include <map>
#include <string>

#include "driver/gpio.h"
#include "esp_log.h"

#include "Tasks/TaskBase.h"
#include "Tasks/SimpleTaskData.h"
#include "Tasks/TaskProgressUpdate.h"
#include "Tasks/TaskController.h"

#include "Sensors/SensorDefs.h"
#include "secrets.h"

class TaskWaterPlants : public TaskBase {

public:
    TaskWaterPlants();

    int getUid() override { return TASK_WATER_PLANTS; }

    void loop() override;
    void run() override;

    static constexpr gpio_num_t pinWaterPump = GPIO_NUM_4;
    static constexpr gpio_num_t pinElectricValve1 = GPIO_NUM_5;
    static constexpr gpio_num_t pinElectricValve2 = GPIO_NUM_18;

    static constexpr uint8_t relayOnState = 0;
    static constexpr uint8_t relayOffState = 1;

private:
    std::map<int, std::string> parametersValues;

    void openWaterValve();
    void closeWaterValve();
    void startWatering();
    void stopWatering();

    void executeTask(SimpleTaskData& taskData) override;
};

#endif
