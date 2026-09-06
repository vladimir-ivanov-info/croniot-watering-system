#include <gtest/gtest.h>

#include "Sensors/SensorWiFiInfoLogic.h"

using namespace SensorWiFiInfoLogic;

TEST(IsValidRssi, MinusTwentyIsValid) {
    EXPECT_TRUE(isValidRssi(-20));
}

TEST(IsValidRssi, MinusOneTwentySevenIsValid) {
    EXPECT_TRUE(isValidRssi(-127));
}

TEST(IsValidRssi, MinusNineteenIsTooWeakSignalRange) {
    EXPECT_FALSE(isValidRssi(-19));
}

TEST(IsValidRssi, MinusOneTwentyEightIsOutOfRange) {
    EXPECT_FALSE(isValidRssi(-128));
}

TEST(IsValidRssi, ZeroIsInvalid) {
    EXPECT_FALSE(isValidRssi(0));
}

TEST(AverageRssi, EmptyVectorHasZeroValidSamples) {
    auto result = averageRssi({});
    EXPECT_EQ(result.validCount, 0);
    EXPECT_EQ(result.failedCount, 0);
    EXPECT_EQ(result.average, 0);
}

TEST(AverageRssi, AllInvalidSamplesGivesZeroValidCount) {
    auto result = averageRssi({0, -5, -200});
    EXPECT_EQ(result.validCount, 0);
    EXPECT_EQ(result.failedCount, 3);
}

TEST(AverageRssi, MixedValidAndInvalidOnlyAveragesValid) {
    auto result = averageRssi({-50, -60, 0, -70});
    EXPECT_EQ(result.validCount, 3);
    EXPECT_EQ(result.failedCount, 1);
    EXPECT_EQ(result.average, (-50 + -60 + -70) / 3);
}

TEST(AverageRssi, AllValidSamples) {
    auto result = averageRssi({-40, -50, -60});
    EXPECT_EQ(result.validCount, 3);
    EXPECT_EQ(result.failedCount, 0);
    EXPECT_EQ(result.average, -50);
}
