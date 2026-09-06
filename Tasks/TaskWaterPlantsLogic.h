#ifndef TASKWATERPLANTSLOGIC_H
#define TASKWATERPLANTSLOGIC_H

#include <optional>
#include <string>

// Pure, hardware-free logic extracted from TaskWaterPlants::executeTask so
// it can be unit-tested on host without FreeRTOS/GPIO.
namespace TaskWaterPlantsLogic {

constexpr int kOpenTimeMillis = 8000;
constexpr int kCloseDelayMillis = 1000;
constexpr int kCloseTimeMillis = 8000;

constexpr int kMinDurationSeconds = 1;
constexpr int kMaxDurationSeconds = 600;

// totalDurationMillis = open + watering + closeDelay + close.
int computeTotalDurationMillis(int wateringDurationMillis);

// Percentage of totalDurationMillis elapsed, rounded to 2 decimals.
// Returns 0.0 if totalDurationMillis <= 0.
float computeProgress(int elapsedMillis, int totalDurationMillis);

// Parses the raw duration task-parameter string into whole seconds.
// Returns std::nullopt for empty or entirely non-numeric input.
// NOTE: mirrors std::stoi's leading-numeric-prefix behavior, e.g.
// "30abc" parses as 30 (not rejected) - this is intentionally preserved,
// not "fixed", since it's the existing parsing behavior.
std::optional<int> parseDurationSeconds(const std::string& durationParam);

// Clamps a raw duration (seconds) into [minSeconds, maxSeconds]. The task
// type declares a 1-600s bound as parameter metadata (TasksInitializer.cpp)
// but executeTask() never enforced it - this closes that gap.
int clampDurationSeconds(int rawSeconds,
                          int minSeconds = kMinDurationSeconds,
                          int maxSeconds = kMaxDurationSeconds);

} // namespace TaskWaterPlantsLogic

#endif
