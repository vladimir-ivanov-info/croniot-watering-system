#include <gtest/gtest.h>

#include "Tasks/TaskWaterPlantsLogic.h"

using namespace TaskWaterPlantsLogic;

TEST(ComputeTotalDurationMillis, SumsAllFourPhases) {
    // 8000 (open) + 30000 (watering) + 1000 (close delay) + 8000 (close)
    EXPECT_EQ(computeTotalDurationMillis(30000), 47000);
}

TEST(ComputeProgress, ZeroElapsedIsZeroPercent) {
    EXPECT_FLOAT_EQ(computeProgress(0, 47000), 0.0f);
}

TEST(ComputeProgress, FullElapsedIsHundredPercent) {
    EXPECT_FLOAT_EQ(computeProgress(47000, 47000), 100.0f);
}

TEST(ComputeProgress, EndOfOpenPhaseBoundary) {
    EXPECT_NEAR(computeProgress(8000, 47000), 17.02f, 0.01f);
}

TEST(ComputeProgress, EndOfWateringPhaseBoundary) {
    EXPECT_NEAR(computeProgress(38000, 47000), 80.85f, 0.01f);
}

TEST(ComputeProgress, EndOfCloseDelayPhaseBoundary) {
    EXPECT_NEAR(computeProgress(39000, 47000), 82.98f, 0.01f);
}

TEST(ComputeProgress, ZeroTotalDurationDoesNotDivideByZero) {
    EXPECT_FLOAT_EQ(computeProgress(100, 0), 0.0f);
}

TEST(ParseDurationSeconds, ValidNumericString) {
    auto result = parseDurationSeconds("30");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 30);
}

TEST(ParseDurationSeconds, EmptyStringIsRejected) {
    EXPECT_FALSE(parseDurationSeconds("").has_value());
}

TEST(ParseDurationSeconds, NonNumericStringIsRejected) {
    EXPECT_FALSE(parseDurationSeconds("abc").has_value());
}

TEST(ParseDurationSeconds, NegativeStringParsesAsNegative) {
    // Not rejected here - clampDurationSeconds is responsible for bounding it.
    auto result = parseDurationSeconds("-5");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, -5);
}

TEST(ParseDurationSeconds, TrailingGarbageIsSilentlyTruncated) {
    // Documents std::stoi's existing prefix-parsing quirk: "30abc" -> 30,
    // not rejected. Preserved intentionally, not "fixed", by this refactor.
    auto result = parseDurationSeconds("30abc");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 30);
}

TEST(ParseDurationSeconds, DecimalPointTruncatesToIntegerPrefix) {
    auto result = parseDurationSeconds("12.5");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 12);
}

TEST(ClampDurationSeconds, WithinRangeIsUnchanged) {
    EXPECT_EQ(clampDurationSeconds(300), 300);
}

TEST(ClampDurationSeconds, ZeroClampsUpToMinimum) {
    EXPECT_EQ(clampDurationSeconds(0), kMinDurationSeconds);
}

TEST(ClampDurationSeconds, NegativeClampsUpToMinimum) {
    EXPECT_EQ(clampDurationSeconds(-5), kMinDurationSeconds);
}

TEST(ClampDurationSeconds, OverMaxClampsDownToMaximum) {
    EXPECT_EQ(clampDurationSeconds(601), kMaxDurationSeconds);
}

TEST(ClampDurationSeconds, ExactBoundariesAreUnchanged) {
    EXPECT_EQ(clampDurationSeconds(kMinDurationSeconds), kMinDurationSeconds);
    EXPECT_EQ(clampDurationSeconds(kMaxDurationSeconds), kMaxDurationSeconds);
}
