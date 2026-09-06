#ifndef SENSORBATTERYLOGIC_H
#define SENSORBATTERYLOGIC_H

#include <string>

// Pure, hardware-free logic extracted from SensorBattery so it can be
// unit-tested on host without ESP-IDF/FreeRTOS/I2C.
namespace SensorBatteryLogic {

// Linear interpolation: minVoltage(10.0V) -> 0%, maxVoltage(13.2V) -> 100%.
// Not clamped - see clampPercentage for the call-site clamp behavior.
float voltsToPercentage(float voltage);

// Clamps a percentage value into [0, 100].
float clampPercentage(float percentage);

// Formats a double with at most 2 decimals, trimming trailing zeros
// (e.g. 3.0 -> "3", 3.10 -> "3.1", 3.14159 -> "3.14").
std::string formatMax2Decimals(double value);

} // namespace SensorBatteryLogic

#endif
