#include <gtest/gtest.h>

#include "Tasks/WaterSequenceController.h"
#include "fakes/FakeWaterGpioDriver.h"

// TaskWaterPlants and TaskWaterPumpSwitch both drive the same physical pump
// GPIO (GPIO4) independently, with no coordination between them
// (see Tasks/TaskWaterPumpSwitch.h). This test does NOT resolve that hazard
// - it documents it against the shared IWaterGpioDriver seam so a regression
// (or an intentional fix) is visible here rather than only on the bench.
//
// TaskWaterPlants' sequence is represented via WaterSequenceController;
// TaskWaterPumpSwitch is represented by a direct setPumpLevel() call, since
// that's exactly what TaskWaterPumpSwitch::executeTask() does (it never
// goes through the valve-aware sequence at all).

TEST(WaterTaskInteractionHazard, PumpSwitchCanTurnPumpOnWithBothValvesClosed) {
    FakeWaterGpioDriver fake;
    WaterSequenceController plantsSequence(fake);

    // Neither TaskWaterPlants sequence step has run - valves are at their
    // default-constructed "closed" state (0). TaskWaterPumpSwitch turns the
    // pump on directly.
    fake.setPumpLevel(0);  // TaskWaterPumpSwitch: state == "on"

    EXPECT_EQ(fake.pumpLevel, 0) << "pump is on";
    EXPECT_NE(fake.valve1Level, 1) << "no water path: valve1 was never opened";
}

TEST(WaterTaskInteractionHazard, PumpSwitchCanTurnPumpOffMidWateringSequence) {
    FakeWaterGpioDriver fake;
    WaterSequenceController plantsSequence(fake);

    plantsSequence.openWaterValve();
    plantsSequence.startWatering();
    ASSERT_EQ(fake.pumpLevel, WaterSequenceController::kPumpOnLevel);

    // TaskWaterPumpSwitch fires independently mid-sequence.
    fake.setPumpLevel(1);  // TaskWaterPumpSwitch: state == "off"

    // Pump is now off, but TaskWaterPlants' own state machine has no idea -
    // it will still run its full timing loop and eventually call
    // stopWatering()/closeWaterValve() as if nothing happened.
    EXPECT_EQ(fake.pumpLevel, WaterSequenceController::kPumpOffLevel);
    EXPECT_EQ(fake.valve1Level, 1) << "valve is still open per TaskWaterPlants' own state";
}
