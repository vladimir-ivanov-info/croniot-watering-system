#include "TasksInitializer.h"

void TasksInitializer::initTasks(){
    registerTaskTypes();
    registerTasks();
}

void TasksInitializer::registerTaskTypes(){
  //WATER PLANTS
    ParameterInteger parameter1(static_cast<int>(TASK_WATER_PLANTS_PARAMETER_DURATION), "Duration", "Duration of the watering in seconds", "s", 1, 600);
    ParameterTime parameter2WaterPlants(static_cast<int>(COMMON_TASK_PARAMETER_TIME), "Time", "Time when you want the task to start");

    list<Parameter> parameters1;
    parameters1.push_back(parameter1);
    parameters1.push_back(parameter2WaterPlants);

  TaskType taskType1(static_cast<int>(TASK_WATER_PLANTS), "Water plants", "Water plants for given amount of seconds", parameters1);

  //LED TEST
    ParameterInteger parameterLedTestDuration(1, "Duration", "Duration of the LED turned on, in seconds", "s", 1, 10);
    ParameterTime parameterLedTestTime(static_cast<int>(COMMON_TASK_PARAMETER_TIME), "Time", "Time when you want the task to start"); 

    list<Parameter> parameters1LedTest;
    parameters1LedTest.push_back(parameterLedTestDuration);
    parameters1LedTest.push_back(parameterLedTestTime);

  TaskType taskTypeLedTest(static_cast<int>(TASK_LED_TEST), "LED test", "Turn on the test LED for given amount of seconds", parameters1LedTest);

  //LED SWITCH
    StatefulParameterSwitch parameterLedSwitch("LED Switch", "Switch to turn on the LED");
  TaskType taskTypeLedSwitch(static_cast<int>(TASK_LED_SWITCH), "LED switch", "Turn on and off the test LED", parameterLedSwitch.toParameter());

  //WATER SWITCH
    StatefulParameterSwitch parameterWaterPumpSwitch("Water Switch", "Switch to turn on the water pump");
  TaskType taskTypeWaterPumpSwitch(static_cast<int>(TASK_WATER_PUMP_SWITCH), "Water pump switch", "Turn on and off the water pump", parameterWaterPumpSwitch.toParameter());

  TaskController::instance().addTaskType(taskType1);
  TaskController::instance().addTaskType(taskTypeLedTest);
  TaskController::instance().addTaskType(taskTypeLedSwitch);
  TaskController::instance().addTaskType(taskTypeWaterPumpSwitch);

  TaskController::instance().init();
}

void TasksInitializer::registerTasks(){
  TaskController::instance().addTask(new TaskLedTest());
  TaskController::instance().addTask(new TaskLedSwitch());
  TaskController::instance().addTask(new TaskWaterPlants());
  TaskController::instance().addTask(new TaskWaterPumpSwitch());
}