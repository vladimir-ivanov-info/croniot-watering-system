#include "Sensors/Battery/SensorBatteryLogic.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace SensorBatteryLogic {

float voltsToPercentage(float voltage) {
    const float maxVoltage = 13.2f;
    const float minVoltage = 10.0f;

    const float slope = 100.0f / (maxVoltage - minVoltage);
    return slope * (voltage - minVoltage);
}

float clampPercentage(float percentage) {
    return std::min(100.0f, std::max(0.0f, percentage));
}

std::string formatMax2Decimals(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    std::string s = oss.str();
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    if (!s.empty() && s.back() == '.') {
        s.pop_back();
    }
    return s;
}

} // namespace SensorBatteryLogic
