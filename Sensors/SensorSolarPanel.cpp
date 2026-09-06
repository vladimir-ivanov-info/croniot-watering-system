#include "SensorSolarPanel.h"
#include "Sensors/SensorSolarPanelLogic.h"

static const char* TAG = "SensorSolarPanel";

void SensorSolarPanel::run() {
    xTaskCreate(taskSolar, "SensorSolar", 10000, this, 1, &taskSolarHandle);
}

void SensorSolarPanel::taskSolar(void* pvParameters) {
    auto* self = static_cast<SensorSolarPanel*>(pvParameters);
    ESP_LOGI(TAG, "SensorSolar initialized...");

    while (true) {
        while (self->continueTask) {

            float solarCurrent = self->getSolarCurrent();
            float solarVoltage = self->getSolarVoltage();

            float solarPower = solarVoltage * solarCurrent;
            std::string solarPowerStr = "0";

            if (SensorSolarPanelLogic::shouldReportPower(solarVoltage, solarCurrent)) {
                solarPowerStr = std::to_string(solarPower);
            }

            self->sendSensorData(SENSOR_SOLAR_POWER, solarPowerStr);

            if (solarPowerStr == "0") {
                vTaskDelay(pdMS_TO_TICKS(5000));
            } else {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }
}

float SensorSolarPanel::getSolarCurrent() {
    int adValue = 0;
    int sampleCount = 0;

    for (int i = 0; i < SENSOR_MEASUREMENT_SAMPLES; ++i) {
        int value = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_SOLAR_CURRENT);
        if (value >= 0) {
            adValue += value;
            ++sampleCount;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    if (sampleCount == 0) return 0.0f;

    float averageAdc = static_cast<float>(adValue) / sampleCount;
    return SensorSolarPanelLogic::adcToCurrent(averageAdc);
}

float SensorSolarPanel::getSolarVoltage() {
    int adValue = 0;
    int sampleCount = 0;

    for (int i = 0; i < SENSOR_MEASUREMENT_SAMPLES; ++i) {
        int value = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_SOLAR_VOLTAGE);
        if (value >= 0) {
            adValue += value;
            ++sampleCount;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    if (sampleCount == 0) return 0.0f;

    float averageAdc = static_cast<float>(adValue) / sampleCount;
    return SensorSolarPanelLogic::adcToVoltage(averageAdc);
}
