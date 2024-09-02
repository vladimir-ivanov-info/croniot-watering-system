#ifndef SENSORBATTERY2_H
#define SENSORBATTERY2_H

#include "MQTTManager.h"
#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#if defined(_SAM3XA_) || defined(ARDUINO_ARCH_SAMD)
// The SAM3XA architecture needs to include this library, it is already included automatically on other platforms //
#include <avr/dtostrf.h>  // Needed for the SAM3XA (Arduino Zero)
#endif

#include "ADS1X15.h"

class SensorBattery2 {
public:
    SensorBattery2();

    void run();
    void stop();

private:

    ADS1115 ADS;
    uint8_t ADS_INPUT_BATTERY_CURRENT = 0;
    uint8_t ADS_INPUT_BATTERY_VOLTAGE = 1;
    uint8_t ADS_INPUT_SOLAR_VOLTAGE = 2;
    uint8_t ADS_INPUT_SOLAR_CURRENT = 3;
    uint8_t address = 0x48;



    volatile bool continueTask;
    TaskHandle_t taskHandle;

    TaskHandle_t batteryCurrentTaskHandle;
    TaskHandle_t batteryVoltageTaskHandle;

    QueueHandle_t batteryCurrentValueQueue;  // Queue handle for passing sensor values
    QueueHandle_t sensorVoltageQueue;  // Queue handle for passing sensor values

    QueueHandle_t solarCurrentQueue;

    EventGroupHandle_t eventGroup;   // Event group for task synchronization
    EventGroupHandle_t eventGroupSolar;   // Event group for task synchronization

    static float voltsToPercentage(float voltage);

    // Static function for the RTOS task
    static void taskFunction(void *param);
    static void measureBatteryCurrent(void *param);

    static int16_t measureADSPin(ADS1115 ADS, int pin);

    static String analogValueToBatteryCurrent(int16_t analogValue);

    static double calculateCurrent(int adcValue);
    static double calculateVoltage(int adcValue);

};

#endif
