#ifndef ADS1115CONTROLLER_H
#define ADS1115CONTROLLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "ADS1X15.h"

class ADS1115Controller{

    public:
        static ADS1115Controller& instance() {
            static ADS1115Controller _instance;
            return _instance;
        }

        int readValueBlocking(int pin);

    private:

        ADS1115Controller();

        ADS1115 ads;
        SemaphoreHandle_t mutex; 

};

#endif