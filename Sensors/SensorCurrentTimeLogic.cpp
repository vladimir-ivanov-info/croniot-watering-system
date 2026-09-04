#include "Sensors/SensorCurrentTimeLogic.h"

#include <cstdio>

namespace SensorCurrentTimeLogic {

std::string formatHms(int hour, int minute, int second) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hour, minute, second);
    return std::string(buffer);
}

} // namespace SensorCurrentTimeLogic
