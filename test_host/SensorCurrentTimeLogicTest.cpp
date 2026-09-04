#include <gtest/gtest.h>

#include "Sensors/SensorCurrentTimeLogic.h"

using SensorCurrentTimeLogic::formatHms;

TEST(FormatHms, Midnight) {
    EXPECT_EQ(formatHms(0, 0, 0), "00:00:00");
}

TEST(FormatHms, EndOfDay) {
    EXPECT_EQ(formatHms(23, 59, 59), "23:59:59");
}

TEST(FormatHms, SingleDigitsArePadded) {
    EXPECT_EQ(formatHms(1, 2, 3), "01:02:03");
}

TEST(FormatHms, Noon) {
    EXPECT_EQ(formatHms(12, 0, 0), "12:00:00");
}
