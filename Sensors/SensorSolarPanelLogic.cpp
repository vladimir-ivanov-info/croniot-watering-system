#include "Sensors/SensorSolarPanelLogic.h"

namespace SensorSolarPanelLogic {

float adcToCurrent(float averageAdcCounts) {
    constexpr float zeroCurrentVoltage = 2.5f;
    constexpr float sensitivity = 0.1f;

    float voltage = averageAdcCounts * 0.0001875f;
    return (voltage - zeroCurrentVoltage) / sensitivity;
}

float adcToVoltage(float averageAdcCounts) {
    double measuredVoltage = averageAdcCounts * 0.0001875;
    return static_cast<float>((measuredVoltage / 5.0) * 25.0);
}

bool shouldReportPower(float voltage, float current) {
    return voltage > 14.0f || current >= 0.7f;
}

} // namespace SensorSolarPanelLogic
