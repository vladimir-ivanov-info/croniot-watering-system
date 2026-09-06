#include "Tasks/TaskWaterPlantsLogic.h"

#include <algorithm>
#include <charconv>
#include <cmath>

namespace TaskWaterPlantsLogic {

int computeTotalDurationMillis(int wateringDurationMillis) {
    return kOpenTimeMillis + wateringDurationMillis + kCloseDelayMillis + kCloseTimeMillis;
}

float computeProgress(int elapsedMillis, int totalDurationMillis) {
    if (totalDurationMillis <= 0) return 0.0f;
    float progress = static_cast<float>(elapsedMillis) * 100.0f / totalDurationMillis;
    return std::round(progress * 100) / 100;
}

std::optional<int> parseDurationSeconds(const std::string& durationParam) {
    // std::from_chars (not std::stoi+try/catch): this project builds with
    // C++ exceptions disabled (CONFIG_COMPILER_CXX_EXCEPTIONS=n), so a
    // throwing parse here would call std::terminate, not "return nullopt".
    if (durationParam.empty()) return std::nullopt;

    int value = 0;
    const char* begin = durationParam.data();
    const char* end = begin + durationParam.size();
    auto result = std::from_chars(begin, end, value);
    if (result.ec != std::errc()) return std::nullopt;

    return value;
}

int clampDurationSeconds(int rawSeconds, int minSeconds, int maxSeconds) {
    return std::clamp(rawSeconds, minSeconds, maxSeconds);
}

} // namespace TaskWaterPlantsLogic
