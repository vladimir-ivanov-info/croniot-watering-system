#include "Sensors/Battery/SensorBattery.h"

void SensorBattery::run(){
    xTaskCreatePinnedToCore(
        taskBattery,      // Function to run the task
        "SensorBattery",          // Task name
        4096,             // Stack size (words)
        this,              // Task parameter (pass the instance of the singleton)
        1,                 // Task priority
        &taskBatteryHandle,       // Task handle
        0                  // Core to run the task (0 or 1)
    );
}

void SensorBattery::taskBattery(void* pvParameters) {
    SensorBattery* self = static_cast<SensorBattery*>(pvParameters);

    Serial.println("SensorBattery initialized...");

    while (true) {
        while(self->continueTask){

                float batteryCurrent = self->getBatteryCurrent();
                float batteryVoltage = self->getBatteryVoltage();

                //BATTERY PERCENTAGE
                float voltageBatteryPercentage = voltsToPercentage(String(batteryVoltage));
                if(voltageBatteryPercentage > 100.0f){
                    voltageBatteryPercentage = 100.0f;
                }
 
                String batteryPercentageStr = String(voltageBatteryPercentage);

                if(voltageBatteryPercentage < 0.0f){
                    batteryPercentageStr = "0";
                }

                //BATTERY POWER
                double batteryPower = batteryVoltage * batteryCurrent;

                String batteryPowerStr = String(batteryPower);
                if(batteryPower < 0.0f){
                    batteryPowerStr = "0";
                }

                self->sendSensorData(static_cast<String>(DEVICE_UUID), static_cast<int>(SENSOR_BATTERY_PERCENTAGE), batteryPercentageStr);
                self->sendSensorData(static_cast<String>(DEVICE_UUID), static_cast<int>(SENSOR_BATTERY_POWER_CONSUMPTION), batteryPowerStr);

                vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}


float SensorBattery::getBatteryCurrent(){
    const float ACS712_ZeroCurrentVoltage = 2.5; // 2.5V at 0A

    //const float ACS712_Sensitivity = 0.1875; // 66mV/A 187.5 for 5Amps current sensor
    const float ACS712_Sensitivity = 0.1; // For 20A

    int adValueCurrent = 0;
    int finalSamplesCurrent = 0; //TODO handle when this variable ends up being 0

    for(int i = 0; i < SENSOR_MESAUREMENT_SAMPLES; i++){
        int adcValue = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_BATTERY_CURRENT);

        if(adcValue < 0){

        } else {
            adValueCurrent += adcValue;
            finalSamplesCurrent++;
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    float adcValueCurrent = (float)adValueCurrent / (float)finalSamplesCurrent;
    float voltage = adcValueCurrent * 0.0001875f; // Convert mV to V
    float current = (voltage - ACS712_ZeroCurrentVoltage) / ACS712_Sensitivity;

    //Serial.print("Voltage: "); Serial.print(voltage); Serial.print(" current: "); Serial.println(current);

    return current;
}

float SensorBattery::getBatteryVoltage(){
    int adValueVoltage = 0;
    
    int finalSamples = 0;

    for(int i = 0; i < SENSOR_MESAUREMENT_SAMPLES; i++){
        int adcValueVoltageNow = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_BATTERY_VOLTAGE);

        if(adcValueVoltageNow < 0){

        } else {
            adValueVoltage += adcValueVoltageNow;
            finalSamples++;
        }

        vTaskDelay(20 / portTICK_PERIOD_MS); // Delay for 1000ms
    }

    float scaledBatteryVoltage = 0.0f;

    if(finalSamples > 0){
        int16_t adcValueVoltage = adValueVoltage / finalSamples;
        double measuredBatteryVoltage = adcValueVoltage * 0.0001875;
        scaledBatteryVoltage = (measuredBatteryVoltage / 5.0) * 25.0;
    }

    return scaledBatteryVoltage;
}

float SensorBattery::voltsToPercentage(String voltsStr){
    float voltage = voltsStr.toDouble();

    float maxVoltage = 13.2;
    float minVoltage = 10.0;

    float minPercentage = 0.0;
    float maxPercentage = 100.0;

    float slope = (maxPercentage - minPercentage) / (maxVoltage - minVoltage);
    float percentage = slope * (voltage - minVoltage) + minPercentage;

    return percentage;
}