#ifndef ESP32WATERGPIODRIVER_H
#define ESP32WATERGPIODRIVER_H

#include "driver/gpio.h"
#include "Tasks/IWaterGpioDriver.h"

// Real ESP-IDF implementation of IWaterGpioDriver, wrapping gpio_set_level.
// A pin passed as GPIO_NUM_NC ("not connected") makes the corresponding
// setter a no-op - used by callers that only ever drive a subset of pins
// (e.g. TaskWaterPumpSwitch only ever calls setPumpLevel).
class Esp32WaterGpioDriver : public IWaterGpioDriver {
public:
    Esp32WaterGpioDriver(gpio_num_t pumpPin, gpio_num_t valve1Pin, gpio_num_t valve2Pin)
        : pumpPin_(pumpPin), valve1Pin_(valve1Pin), valve2Pin_(valve2Pin) {}

    void setPumpLevel(int level) override {
        if (pumpPin_ != GPIO_NUM_NC) gpio_set_level(pumpPin_, level);
    }

    void setValve1Level(int level) override {
        if (valve1Pin_ != GPIO_NUM_NC) gpio_set_level(valve1Pin_, level);
    }

    void setValve2Level(int level) override {
        if (valve2Pin_ != GPIO_NUM_NC) gpio_set_level(valve2Pin_, level);
    }

private:
    gpio_num_t pumpPin_;
    gpio_num_t valve1Pin_;
    gpio_num_t valve2Pin_;
};

#endif
