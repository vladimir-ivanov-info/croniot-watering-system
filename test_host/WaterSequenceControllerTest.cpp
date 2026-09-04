#include <gtest/gtest.h>

#include "Tasks/WaterSequenceController.h"
#include "fakes/FakeWaterGpioDriver.h"

using Call = FakeWaterGpioDriver::Call;

TEST(WaterSequenceController, OpenWaterValveSetsValve1OpenValve2Closed) {
    FakeWaterGpioDriver fake;
    WaterSequenceController sequence(fake);

    sequence.openWaterValve();

    std::vector<Call> expected = {{"valve1", 1}, {"valve2", 0}};
    EXPECT_EQ(fake.calls, expected);
}

TEST(WaterSequenceController, CloseWaterValveSetsValve2ClosedValve1Off) {
    FakeWaterGpioDriver fake;
    WaterSequenceController sequence(fake);

    sequence.closeWaterValve();

    std::vector<Call> expected = {{"valve2", 1}, {"valve1", 0}};
    EXPECT_EQ(fake.calls, expected);
}

TEST(WaterSequenceController, StartWateringTurnsPumpOn) {
    FakeWaterGpioDriver fake;
    WaterSequenceController sequence(fake);

    sequence.startWatering();

    EXPECT_EQ(fake.pumpLevel, WaterSequenceController::kPumpOnLevel);
}

TEST(WaterSequenceController, StopWateringTurnsPumpOff) {
    FakeWaterGpioDriver fake;
    WaterSequenceController sequence(fake);

    sequence.stopWatering();

    EXPECT_EQ(fake.pumpLevel, WaterSequenceController::kPumpOffLevel);
}

TEST(WaterSequenceController, FullSequenceCallOrderAndPinStates) {
    FakeWaterGpioDriver fake;
    WaterSequenceController sequence(fake);

    sequence.openWaterValve();
    sequence.startWatering();
    sequence.stopWatering();
    sequence.closeWaterValve();

    std::vector<Call> expected = {
        {"valve1", 1}, {"valve2", 0},  // open
        {"pump", 0},                    // start (kPumpOnLevel)
        {"pump", 1},                    // stop (kPumpOffLevel)
        {"valve2", 1}, {"valve1", 0},  // close
    };
    EXPECT_EQ(fake.calls, expected);
}

TEST(WaterSequenceController, DriveToSafeStateEndsWithEverythingOff) {
    FakeWaterGpioDriver fake;
    WaterSequenceController sequence(fake);

    sequence.driveToSafeState();

    // Final state, regardless of the intermediate valve2 set-then-override.
    EXPECT_EQ(fake.valve1Level, 0);
    EXPECT_EQ(fake.valve2Level, 0);
    EXPECT_EQ(fake.pumpLevel, WaterSequenceController::kPumpOffLevel);
}
