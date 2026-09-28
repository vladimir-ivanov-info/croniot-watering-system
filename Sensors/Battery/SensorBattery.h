#ifndef SENSORBATTERY_H
#define SENSORBATTERY_H

#include "Messages/MessageSensorData.h"
#include "Sensors/ReportPolicy.h"
#include "Sensors/SensorDefs.h"
#include "Sensors/Sensor.h"
#include "Sensors/ADS1115Controller.h"
#include "secrets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class SensorBattery : public Sensor {
public:
    // Plan §7.2's own example (battery->setReporting(1000,
    // ReportPolicy::Batch(60))) - battery percentage changes slowly, so
    // 60 readings (one minute at this sensor's 3s sample period) in a
    // single batch is a real bandwidth win with no loss of resolution
    // that matters. Battery *power* stays on the legacy immediate path
    // below (sendSensorData) - deliberately not batched, to demonstrate
    // (and actually use) both paths from the same Sensor subclass.
    SensorBattery() { setReporting(3000, croniot::ReportPolicy::Batch(/*periodSec=*/60)); }

    void run() override;

    void stop() {
        continueTask = false;
        if (taskBatteryHandle != NULL) {
            vTaskDelete(taskBatteryHandle);
            taskBatteryHandle = NULL;
        }
    }

private:
    const uint8_t ADS_INPUT_BATTERY_CURRENT = 0;
    const uint8_t ADS_INPUT_BATTERY_VOLTAGE = 1;
    const int SENSOR_MESAUREMENT_SAMPLES = 20;

    TaskHandle_t taskBatteryHandle = NULL;
    static void taskBattery(void* param);

    volatile bool continueTask = true;

    float getBatteryCurrent();
    float getBatteryVoltage();
};

#endif
