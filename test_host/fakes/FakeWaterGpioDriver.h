#ifndef FAKE_WATER_GPIO_DRIVER_H
#define FAKE_WATER_GPIO_DRIVER_H

#include <string>
#include <vector>

#include "Tasks/IWaterGpioDriver.h"

// Test-only fake: records every GPIO call in order instead of touching
// real hardware, so tests can assert exact call sequences/pin states.
class FakeWaterGpioDriver : public IWaterGpioDriver {
public:
    struct Call {
        std::string pin;  // "pump", "valve1" or "valve2"
        int level;

        bool operator==(const Call& other) const {
            return pin == other.pin && level == other.level;
        }
    };

    void setPumpLevel(int level) override {
        calls.push_back({"pump", level});
        pumpLevel = level;
    }

    void setValve1Level(int level) override {
        calls.push_back({"valve1", level});
        valve1Level = level;
    }

    void setValve2Level(int level) override {
        calls.push_back({"valve2", level});
        valve2Level = level;
    }

    std::vector<Call> calls;
    int pumpLevel = -1;
    int valve1Level = -1;
    int valve2Level = -1;
};

#endif
