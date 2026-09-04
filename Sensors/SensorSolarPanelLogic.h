#ifndef SENSORSOLARPANELLOGIC_H
#define SENSORSOLARPANELLOGIC_H

// Pure, hardware-free logic extracted from SensorSolarPanel so it can be
// unit-tested on host without ESP-IDF/FreeRTOS/I2C. These pin the CURRENT
// calibration formulas as regression baselines - they are not re-derived.
namespace SensorSolarPanelLogic {

// Converts an averaged ADS1115 raw ADC count (solar current channel) to amps.
float adcToCurrent(float averageAdcCounts);

// Converts an averaged ADS1115 raw ADC count (solar voltage channel) to volts.
float adcToVoltage(float averageAdcCounts);

// Whether the panel is producing enough power to be worth reporting
// (mirrors the original inline threshold: voltage > 14.0f || current >= 0.7f).
bool shouldReportPower(float voltage, float current);

} // namespace SensorSolarPanelLogic

#endif
