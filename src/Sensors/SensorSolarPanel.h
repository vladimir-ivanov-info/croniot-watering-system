#ifndef SENSORSOLARPANEL_H
#define SENSORSOLARPANEL_H

#include "MQTTManager.h"
#include "Messages/MessageSensorData.h"
#include "Sensors/SensorDefs.h"

#if ARDUINO >= 100  // Arduino IDE versions before 100 need to use the older library
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#if defined(_SAM3XA_) || defined(ARDUINO_ARCH_SAMD)
// The SAM3XA architecture needs to include this library, it is already included automatically on
// other platforms //
#include <avr/dtostrf.h>  // Needed for the SAM3XA (Arduino Zero)
#endif

#include "ADS1X15.h"

class SensorSolarPanel{
    
    public:
        SensorSolarPanel(){}

        void run();
    private:

        TaskHandle_t taskHandle = NULL;
        
        static float voltsToPercentage(String voltsStr){
            float voltage = voltsStr.toDouble();

            float maxVoltage = 13.2;
            float minVoltage = 10.0;

            float minPercentage = 0.0;
            float maxPercentage = 100.0;

            float slope = (maxPercentage - minPercentage) / (maxVoltage - minVoltage);
            float percentage = slope * (voltage - minVoltage) + minPercentage;

            return percentage;
        }

        // Static function for the RTOS task
        static void taskFunction(void *param) {
            SensorSolarPanel *self = (SensorSolarPanel *)param;

            ADS1115 ADS[4];
            uint16_t val[16];
            int idx = 1;

            uint32_t last = 0, now = 0;

            Wire.begin();


            uint8_t i = 0;

            uint8_t address = 0x48;

            ADS[i] = ADS1115(address);

            Serial.print(address, HEX);
            Serial.print("  ");
            Serial.println(ADS[i].begin() ? "connected" : "not connected");

            //  0 = slow   4 = medium   7 = fast, but more noise
            ADS[i].setDataRate(0);

           if (ADS[i].isConnected()) ADS[i].requestADC(idx);

            int sensorSolarPower = static_cast<int>(SENSOR_SOLAR_POWER);
            Serial.print("\n\nDisplay INA Readings V1.0.8\n");
            Serial.print(" - Searching & Initializing INA devices\n");

            for(;;){
                static uint16_t loopCounter = 0;     // Count the number of iterations
                static char     sprintfBuffer[100];  // Buffer to format output
                static char     busChar[8], shuntChar[10], busAChar[10], busWChar[10];  // Output buffers

                String voltage = String(busChar);
                voltage.trim();
                double batteryPercentage = voltsToPercentage(voltage);
                String batteryPercentageStr = String(batteryPercentage);

                if (ADS[i].isConnected() && ADS[i].isBusy()){ 
                        
                } else {
                    ADS[i].requestADC(idx);
                    int16_t value = ADS[i].readADC(1);

                    int16_t correctedValue = value - 13.333; //equivalent to 2.5V with 6.144V max vin for DS1115
                    float correctedVoltage = ADS[i].toVoltage(correctedValue);
                    float correctedVoltage2 = correctedVoltage  - 2.5F;
                    
                    double amps = correctedVoltage2*1000.f / 66.f; //66 vM per amp for 30A ACS712

                    double wattageDouble = voltage.toDouble()*amps;
                    String wattage = String(wattageDouble);

                    Serial.print("Solar Amps: "); Serial.print(amps); Serial.print(" A"); Serial.print("Solar Wattage: "); Serial.print(wattage); Serial.println(" W");
                    String topicSolarPowerConsumption = "esp32uuid_watering_system/sensor_data/" + String(sensorSolarPower);
                    MQTTManager::instance().publish(topicSolarPowerConsumption.c_str(), wattage.c_str());
                }

                vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000ms
            }    
        }
};
#endif

