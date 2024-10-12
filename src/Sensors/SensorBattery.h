#ifndef SENSORBATTERY_H
#define SENSORBATTERY_H

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

#include "Sensors/Sensor.h"
#include "secrets.h"

//TODO move to .cpp
class SensorBattery : public Sensor {
    
    public:
        SensorBattery(){}

        void run() override;
        void stop() {
            continueTask = false;
            if (taskHandle != NULL) {
                vTaskDelete(taskHandle);
                taskHandle = NULL;
            }
        }

    private:

        volatile bool continueTask = true;

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
            SensorBattery *self = (SensorBattery *)param;

            ADS1115 ADS;
            
            uint16_t val[16];
            int idx = 0;

            uint32_t last = 0, now = 0;

            Wire.begin();

            uint8_t ADS_INPUT_BATTERY_CURRENT = 0;
            uint8_t ADS_INPUT_BATTERY_VOLTAGE = 1;
            uint8_t ADS_INPUT_SOLAR_VOLTAGE = 2;
            uint8_t ADS_INPUT_SOLAR_CURRENT = 3;
            uint8_t address = 0x48;

            ADS = ADS1115(address);

            Serial.print(address, HEX);
            Serial.print("  ");
            Serial.println(ADS.begin() ? "ADS connected" : "not connected");

            //  0 = slow   4 = medium   7 = fast, but more noise
            ADS.setDataRate(4); //Look up: when I used 0, at some point the read() started returning -101, which is ERROR_TIMEOUT. After changing to 4 it works fine.
            //ADS.setGain(1); //+- 4.096 V
            ADS.setGain(0); //± 6.144 V

            if (ADS.isConnected()){
              ADS.requestADC(ADS_INPUT_BATTERY_CURRENT);
              ADS.requestADC(ADS_INPUT_BATTERY_VOLTAGE);
              ADS.requestADC(ADS_INPUT_SOLAR_VOLTAGE);
              ADS.requestADC(ADS_INPUT_SOLAR_CURRENT);
            } 
            /////////////////////

            int sensorBatteryPercentage = static_cast<int>(SENSOR_BATTERY_PERCENTAGE);
            int sensorBatteryPowerConsumption = static_cast<int>(SENSOR_BATTERY_POWER_CONSUMPTION);
            
            int sensorSolarPower = static_cast<int>(SENSOR_SOLAR_POWER);

            while(self->continueTask){
              if (ADS.isConnected() && ADS.isBusy()){
                
              } else {
                  ADS.requestADC(ADS_INPUT_BATTERY_CURRENT);
                  ADS.requestADC(ADS_INPUT_BATTERY_VOLTAGE);
                  ADS.requestADC(ADS_INPUT_SOLAR_VOLTAGE);
                  ADS.requestADC(ADS_INPUT_SOLAR_CURRENT);

                  const float ACS712_ZeroCurrentVoltage = 2.5; // 2.5V at 0A

                  //const float ACS712_Sensitivity = 0.1875; // 66mV/A 187.5 for 5Amps current sensor
                  const float ACS712_Sensitivity = 0.1; // For 20A

                  int adValueCurrent = 0;
                  int samplesAdcCurrent = 50;
                  int finalSamplesCurrent = 0;

                  for(int i = 0; i < samplesAdcCurrent; i++){
                    int16_t adcValue = ADS.readADC(ADS_INPUT_BATTERY_CURRENT); // ads.readADC_SingleEnded(0);
                    //Serial.print("Value amp voltage:"); Serial.println(adcValue); //13318
                    if(adcValue < 0){

                    } else {
                      adValueCurrent += adcValue;
                      finalSamplesCurrent++;
                    }
                    vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for 1000ms
                  }


                  float adcValueCurrent = (float)adValueCurrent / (float)finalSamplesCurrent;

                  float voltage = adcValueCurrent * 0.0001875f; // Convert mV to V

                  float current = (voltage - ACS712_ZeroCurrentVoltage) / ACS712_Sensitivity;

                  int adValueVoltage = 0;
                  
                  int finalSamples = 0;

                  for(int i = 0; i < samplesAdcCurrent; i++){
                    int16_t adcValueVoltageNow = ADS.readADC(ADS_INPUT_BATTERY_VOLTAGE); // ads.readADC_SingleEnded(0);

                    if(adcValueVoltageNow < 0){

                    } else {
                      adValueVoltage += adcValueVoltageNow;
                      finalSamples++;
                    }

                    vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for 1000ms
                  }

                  int16_t adcValueVoltage = adValueVoltage / finalSamples;
                
                  double measuredBatteryVoltage = adcValueVoltage * 0.0001875;

                  double scaledBatteryVoltage = (measuredBatteryVoltage / 5.0) * 25.0;

                  //BATTERY PERCENTAGE
                  float voltageBatteryPercentage = voltsToPercentage(String(scaledBatteryVoltage));
                  String batteryPercentageStr = String(voltageBatteryPercentage);

                  //BATTERY POWER
                  double batteryPower = scaledBatteryVoltage * current;
                  String batteryPowerStr = String(batteryPower);

                  String topicBatteryPercentage = "/" + static_cast<String>(DEVICE_UUID) + "/sensor_data/" + String(sensorBatteryPercentage);
                  MQTTManager::instance().publish(topicBatteryPercentage.c_str(), batteryPercentageStr.c_str());
                  String topicBatteryPowerConsumption = "/" + static_cast<String>(DEVICE_UUID) + "/sensor_data/" + String(sensorBatteryPowerConsumption);

                  MQTTManager::instance().publish(topicBatteryPowerConsumption.c_str(), batteryPowerStr.c_str());

                  /* SOLAR */
                  //SOLAR AMPS
                  int adValueSolar = 0;
                  int samplesAdcSolar = 50;

                  for(int i = 0; i < samplesAdcSolar; i++){
                    int16_t adcValueSolarNow =  ADS.readADC(ADS_INPUT_SOLAR_CURRENT); // ads.readADC_SingleEnded(0);
                    adValueSolar += adcValueSolarNow;
                    vTaskDelay(10 / portTICK_PERIOD_MS); // Delay for 1000ms
                  }

                  int16_t adcValueSolar = adValueSolar / samplesAdcSolar;


                  float voltageSolar = adcValueSolar * 0.0001875; // Convert mV to V

                  float currentSolar = (voltageSolar - ACS712_ZeroCurrentVoltage) / ACS712_Sensitivity;

                  String solarPowerStr = "";

                  if(currentSolar < 0){
                    solarPowerStr = "0";
                  } else {
                    //SOLAR VOLTAGE
                    int16_t adcValueSolarVoltage =  ADS.readADC(ADS_INPUT_SOLAR_VOLTAGE);
                    double measuredBatteryVoltageSolar = adcValueSolarVoltage * 0.0001875;
                    double scaledVoltageSolar = (measuredBatteryVoltageSolar / 5.0) * 25.0;

                    //SOLAR POWER
                    double solarPower = scaledVoltageSolar * currentSolar;
                    solarPowerStr = String(solarPower);
                  }
                
                 // String topicSensorPower = "/" +  static_cast<String>(DEVICE_UUID) + "_outcoming/sensor_data/" + String(sensorSolarPower);
                  String topicSensorPower = "/" + static_cast<String>(DEVICE_UUID) + "/sensor_data/" + String(sensorSolarPower);

                  MQTTManager::instance().publish(topicSensorPower.c_str(), solarPowerStr.c_str());
              }
              vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            Serial.println("Sensor stopped");
            vTaskDelete(NULL);
        }
};
#endif

