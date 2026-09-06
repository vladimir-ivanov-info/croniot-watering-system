#ifndef WATERSEQUENCECONTROLLER_H
#define WATERSEQUENCECONTROLLER_H

#include "Tasks/IWaterGpioDriver.h"

// Pure orchestration of the valve/pump GPIO sequence, decoupled from
// FreeRTOS/timing (that stays in TaskWaterPlants::executeTask) and from the
// concrete GPIO implementation (injected via IWaterGpioDriver). This class
// has zero ESP-IDF/FreeRTOS includes, so it's fully unit-testable on host
// with a fake driver (see test_host/fakes/FakeWaterGpioDriver.h).
class WaterSequenceController {
public:
    explicit WaterSequenceController(IWaterGpioDriver& driver) : driver_(driver) {}

    // Drives valve1/valve2/pump to the safe "closed, pump off" state.
    // Mirrors TaskWaterPlants' original constructor sequence exactly,
    // including the intermediate valve2 set-then-override.
    void driveToSafeState() {
        driver_.setValve2Level(1);
        driver_.setValve1Level(0);
        driver_.setPumpLevel(kPumpOffLevel);

        // DISCONNECT
        driver_.setValve2Level(0);
        driver_.setValve1Level(0);
    }

    void openWaterValve() {
        driver_.setValve1Level(1);
        driver_.setValve2Level(0);
    }

    void closeWaterValve() {
        driver_.setValve2Level(1);
        driver_.setValve1Level(0);
    }

    void startWatering() {
        driver_.setPumpLevel(kPumpOnLevel);
    }

    void stopWatering() {
        driver_.setPumpLevel(kPumpOffLevel);
    }

    static constexpr int kPumpOnLevel = 0;
    static constexpr int kPumpOffLevel = 1;

private:
    IWaterGpioDriver& driver_;
};

#endif
