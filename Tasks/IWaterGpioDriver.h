#ifndef IWATERGPIODRIVER_H
#define IWATERGPIODRIVER_H

// Abstraction over the GPIO pins that drive the water pump relay and the
// two solenoid valves. Lets the watering sequence logic (WaterSequenceController)
// be unit-tested on host with a fake implementation, with zero ESP-IDF dependency.
class IWaterGpioDriver {
public:
    virtual ~IWaterGpioDriver() = default;

    virtual void setPumpLevel(int level) = 0;
    virtual void setValve1Level(int level) = 0;
    virtual void setValve2Level(int level) = 0;
};

#endif
