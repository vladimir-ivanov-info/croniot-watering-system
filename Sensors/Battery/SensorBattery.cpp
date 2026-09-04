#include "Sensors/Battery/SensorBattery.h"
#include "Sensors/Battery/SensorBatteryLogic.h"
#include "esp_log.h"
#include <cmath>
#include <string>

static const char* TAG = "SensorBattery";
static constexpr float MV_TO_V = 0.001f;


static constexpr float ACS712_ZERO_CURRENT_VOLTAGE = 2.516f;
// Cambia según modelo: 0.185 (5A), 0.100 (20A), 0.066 (30A)
static constexpr float ACS712_SENSITIVITY_V_PER_A = 0.100f;

void SensorBattery::run() {
    xTaskCreatePinnedToCore(
        taskBattery,
        "SensorBattery",
        4096,
        this,
        1,
        &taskBatteryHandle,
        0
    );

    //TODO: solve i2c errors later. First connect sda,scl. xTaskCreate(taskBattery, "SensorBattery", 4096, this, 1, &taskBatteryHandle);
}

void SensorBattery::taskBattery(void* pvParameters) {
    SensorBattery* self = static_cast<SensorBattery*>(pvParameters);

    ESP_LOGI(TAG, "SensorBattery initialized...");

    while (self->continueTask) {
        float batteryCurrent = self->getBatteryCurrent();
        float batteryVoltage = self->getBatteryVoltage();

        float voltageBatteryPercentage = SensorBatteryLogic::voltsToPercentage(batteryVoltage);
        voltageBatteryPercentage = SensorBatteryLogic::clampPercentage(voltageBatteryPercentage);

        const float batteryCurrentAbs = std::fabs(batteryCurrent);
        double batteryPower = batteryVoltage * batteryCurrentAbs;
        const std::string batteryPercentageStr = SensorBatteryLogic::formatMax2Decimals(voltageBatteryPercentage);
        const std::string batteryPowerStr = SensorBatteryLogic::formatMax2Decimals(batteryPower);
        const std::string batteryVoltageStr = SensorBatteryLogic::formatMax2Decimals(batteryVoltage);

        self->sendSensorData(SENSOR_BATTERY_PERCENTAGE, batteryPercentageStr);
        self->sendSensorData(SENSOR_BATTERY_POWER_CONSUMPTION, batteryPowerStr);

        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    ESP_LOGI(TAG, "SensorBattery task stopped");
    vTaskDelete(nullptr);
}

float SensorBattery::getBatteryCurrent() {
    int adValueCurrent = 0;
    int finalSamplesCurrent = 0;

    for (int i = 0; i < SENSOR_MESAUREMENT_SAMPLES; ++i) {
        int adcValue = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_BATTERY_CURRENT);
        if (adcValue >= 0) {
            adValueCurrent += adcValue;
            finalSamplesCurrent++;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    if (finalSamplesCurrent == 0) return 0.0f;

    const float avgMv = static_cast<float>(adValueCurrent) / finalSamplesCurrent;
    const float voltage = avgMv * MV_TO_V; // sample_mv ya viene en mV
    const float current = (voltage - ACS712_ZERO_CURRENT_VOLTAGE) / ACS712_SENSITIVITY_V_PER_A;

    return current;
}

float SensorBattery::getBatteryVoltage() {
    int adValueVoltage = 0;
    int finalSamples = 0;

    for (int i = 0; i < SENSOR_MESAUREMENT_SAMPLES; ++i) {
        int adcValue = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_BATTERY_VOLTAGE);
        if (adcValue >= 0) {
            adValueVoltage += adcValue;
            finalSamples++;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    if (finalSamples == 0) return 0.0f;

    const float avgMv = static_cast<float>(adValueVoltage) / finalSamples;
    const float measuredVoltage = avgMv * MV_TO_V; // sample_mv ya viene en mV
    const float scaledVoltage = (measuredVoltage / 5.0f) * 25.0f; // divisor 5:1

    return scaledVoltage;
}
