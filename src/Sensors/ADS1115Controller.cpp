#include "ADS1115Controller.h"

ADS1115Controller::ADS1115Controller(){
    mutex = xSemaphoreCreateMutex();

    Wire.begin();
    uint8_t address = 0x48;
    ads = ADS1115(address);

    Serial.print(address, HEX);
    Serial.print("  ");
    Serial.println(ads.begin() ? "ADS connected" : "not connected");

    //  0 = slow   4 = medium   7 = fast, but more noise
    ads.setDataRate(0);
    ads.setGain(0); //± 6.144 V
}

int ADS1115Controller::readValueBlocking(int pin){

    int value = -1;
    try{
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {

            ads.requestADC(pin);

            while(!ads.isReady()){
                vTaskDelay(20 / portTICK_PERIOD_MS);
            }

            value = ads.getValue();

            xSemaphoreGive(mutex); // Release the mutex
        }
    } catch(const std::exception& e){
        Serial.println(">>>>>>>>>Exception in ADS1115Controller::readValueBlocking");
        Serial.println("<<<<<<<<<<");
        xSemaphoreGive(mutex); // Release the mutex
    }

    return value;
}