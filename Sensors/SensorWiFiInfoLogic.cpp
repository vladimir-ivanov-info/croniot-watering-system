#include "Sensors/SensorWiFiInfoLogic.h"

namespace SensorWiFiInfoLogic {

bool isValidRssi(int rssi) {
    return rssi <= -20 && rssi >= -127;
}

RssiAverageResult averageRssi(const std::vector<int>& samples) {
    RssiAverageResult result;
    int sum = 0;

    for (int rssi : samples) {
        if (isValidRssi(rssi)) {
            sum += rssi;
            result.validCount++;
        } else {
            result.failedCount++;
        }
    }

    result.average = result.validCount > 0 ? sum / result.validCount : 0;
    return result;
}

} // namespace SensorWiFiInfoLogic
