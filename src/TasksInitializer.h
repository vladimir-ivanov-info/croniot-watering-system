#ifndef TASKSINITIALIZER_H
#define TASKSINITIALIZER_H

#include "Tasks/TaskLedTest.h"
#include "Tasks/TaskLedSwitch.h"
#include "Tasks/TaskWaterPlants.h"
#include "Tasks/TaskWaterPumpSwitch.h"

#include "Parameters/StatefulParameterSwitch.h"
#include "Parameters/ParameterInteger.h"
#include "Parameters/ParameterTime.h"

class TasksInitializer {

    public:
        static void initTasks();

    private:
        static void registerTaskTypes();
        static void registerTasks();

};

#endif