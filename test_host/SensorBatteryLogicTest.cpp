#include <gtest/gtest.h>

#include "Sensors/Battery/SensorBatteryLogic.h"

using SensorBatteryLogic::voltsToPercentage;
using SensorBatteryLogic::clampPercentage;
using SensorBatteryLogic::formatMax2Decimals;

TEST(VoltsToPercentage, MinVoltageIsZeroPercent) {
    EXPECT_FLOAT_EQ(voltsToPercentage(10.0f), 0.0f);
}

TEST(VoltsToPercentage, MaxVoltageIsHundredPercent) {
    EXPECT_FLOAT_EQ(voltsToPercentage(13.2f), 100.0f);
}

TEST(VoltsToPercentage, MidpointIsAboutHalf) {
    EXPECT_NEAR(voltsToPercentage(11.6f), 50.0f, 0.1f);
}

TEST(VoltsToPercentage, BelowRangeGoesNegativeUnclamped) {
    EXPECT_LT(voltsToPercentage(9.0f), 0.0f);
}

TEST(VoltsToPercentage, AboveRangeGoesOverHundredUnclamped) {
    EXPECT_GT(voltsToPercentage(14.0f), 100.0f);
}

TEST(ClampPercentage, ClampsBelowZero) {
    EXPECT_FLOAT_EQ(clampPercentage(voltsToPercentage(9.0f)), 0.0f);
}

TEST(ClampPercentage, ClampsAboveHundred) {
    EXPECT_FLOAT_EQ(clampPercentage(voltsToPercentage(14.0f)), 100.0f);
}

TEST(ClampPercentage, PassesThroughInRangeValues) {
    EXPECT_FLOAT_EQ(clampPercentage(50.0f), 50.0f);
}

TEST(FormatMax2Decimals, TrimsTrailingZerosFromWholeNumber) {
    EXPECT_EQ(formatMax2Decimals(3.0), "3");
}

TEST(FormatMax2Decimals, TrimsSingleTrailingZero) {
    EXPECT_EQ(formatMax2Decimals(3.10), "3.1");
}

TEST(FormatMax2Decimals, KeepsTwoSignificantDecimals) {
    EXPECT_EQ(formatMax2Decimals(3.14159), "3.14");
}

TEST(FormatMax2Decimals, ZeroFormatsAsZero) {
    EXPECT_EQ(formatMax2Decimals(0.0), "0");
}

TEST(FormatMax2Decimals, NegativeValue) {
    EXPECT_EQ(formatMax2Decimals(-1.50), "-1.5");
}
