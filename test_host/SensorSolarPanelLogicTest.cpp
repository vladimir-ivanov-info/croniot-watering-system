#include <gtest/gtest.h>

#include "Sensors/SensorSolarPanelLogic.h"

using namespace SensorSolarPanelLogic;

TEST(ShouldReportPower, HighVoltageAloneTriggersReport) {
    EXPECT_TRUE(shouldReportPower(15.0f, 0.1f));
}

TEST(ShouldReportPower, HighCurrentAloneTriggersReport) {
    EXPECT_TRUE(shouldReportPower(5.0f, 0.8f));
}

TEST(ShouldReportPower, BothBelowThresholdDoesNotReport) {
    EXPECT_FALSE(shouldReportPower(5.0f, 0.1f));
}

TEST(ShouldReportPower, VoltageExactlyAtThresholdIsNotEnough) {
    // Strictly greater-than for voltage.
    EXPECT_FALSE(shouldReportPower(14.0f, 0.0f));
}

TEST(ShouldReportPower, VoltageJustAboveThresholdTriggers) {
    EXPECT_TRUE(shouldReportPower(14.01f, 0.0f));
}

TEST(ShouldReportPower, CurrentExactlyAtThresholdTriggers) {
    // Greater-than-or-equal for current.
    EXPECT_TRUE(shouldReportPower(0.0f, 0.7f));
}

TEST(ShouldReportPower, CurrentJustBelowThresholdDoesNotTrigger) {
    EXPECT_FALSE(shouldReportPower(0.0f, 0.699f));
}

TEST(AdcConversion, ZeroCountsGivesZeroVoltsAndNegativeOffsetCurrent) {
    // Pins the current calibration formulas as a regression baseline.
    EXPECT_FLOAT_EQ(adcToVoltage(0.0f), 0.0f);
    EXPECT_NEAR(adcToCurrent(0.0f), -25.0f, 0.01f);  // -zeroCurrentVoltage/sensitivity
}
