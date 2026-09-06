#ifndef SENSORWIFIINFOLOGIC_H
#define SENSORWIFIINFOLOGIC_H

#include <vector>

// Pure, hardware-free logic extracted from SensorWiFiInfo so it can be
// unit-tested on host without ESP-IDF's WiFi driver.
namespace SensorWiFiInfoLogic {

// Expected RSSI range in dBm for WiFi: [-127, -20].
bool isValidRssi(int rssi);

struct RssiAverageResult {
    int average = 0;
    int validCount = 0;
    int failedCount = 0;
};

// Averages only the samples that pass isValidRssi(). If none are valid,
// average is 0 and validCount is 0 (caller decides whether to publish).
RssiAverageResult averageRssi(const std::vector<int>& samples);

} // namespace SensorWiFiInfoLogic

#endif
