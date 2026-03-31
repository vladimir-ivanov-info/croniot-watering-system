#ifndef ADS1115CONTROLLER_H
#define ADS1115CONTROLLER_H

#include "ads1x15.hpp"
#include "i2c.hpp"

#include <memory>

class ADS1115Controller {
public:
    ADS1115Controller();
    ~ADS1115Controller();

    static ADS1115Controller& instance() {
        static ADS1115Controller _instance;
        return _instance;
    }

    int readValueBlocking(int pin);

private:
    std::unique_ptr<espp::I2c> i2c;
    std::unique_ptr<espp::Ads1x15> ads;
};

#endif
