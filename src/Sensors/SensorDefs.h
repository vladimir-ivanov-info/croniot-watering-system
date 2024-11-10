#ifndef SENSORDEFS_H
#define SENSORDEFS_H

#define SENSOR_TEMPERATURE_CPU 1
#define SENSOR_WIFI_STRENGTH 39
#define SENSOR_VOLTAGE_MAIN 40
#define SENSOR_BATTERY_PERCENTAGE 41
#define SENSOR_BATTERY_POWER_CONSUMPTION 42
#define SENSOR_SOLAR_POWER 43


#define TASK_WATER_PLANTS 1
    #define TASK_WATER_PLANTS_PARAMETER_DURATION 1
    //#define TASK_WATER_PLANTS_PARAMETER_TIME 2 --> moved to croniot-iot common constants

#define TASK_LED_TEST 2
#define TASK_LED_SWITCH 3
    #define TASK_LED_SWITCH_PARAMETER_SWITCH_STATE 1

#endif