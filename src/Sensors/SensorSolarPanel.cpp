#include "SensorSolarPanel.h"

void SensorSolarPanel::run(){
    xTaskCreatePinnedToCore(
        taskSolar,      // Function to run the task
        "SensorSolar",          // Task name
        10000,             // Stack size (words)
        this,              // Task parameter (pass the instance of the singleton)
        1,                 // Task priority
        &taskSolarHandle,       // Task handle
        0                  // Core to run the task (0 or 1)
    );
}

void SensorSolarPanel::taskSolar(void* pvParameters) {
    SensorSolarPanel* self = static_cast<SensorSolarPanel*>(pvParameters);

    Serial.println("SensorSolar task initialized...");

    while (true) {
        while(self->continueTask){
            
            float solarCurrent = self->getSolarCurrent();
            //Serial.print("SOLAR CURRENT:"); Serial.println(solarCurrent);

            String solarPowerStr = "";

            float solarVoltage = self->getSolarVoltage();
            //Serial.print("SOLAR VOLTAGE:"); Serial.println(solarVoltage);

            if(solarVoltage <= 14.0f && solarCurrent < 0.7f){
                solarPowerStr = "0";
            } else {
                double solarPower = solarVoltage * solarCurrent;
                solarPowerStr = String(solarPower);
            }

            self->sendSensorData(static_cast<String>(DEVICE_UUID), static_cast<int>(SENSOR_SOLAR_POWER), solarPowerStr);

            if(solarPowerStr == "0"){
                vTaskDelay(5000 / portTICK_PERIOD_MS); //Delay when we detect solar sensor not connected or almost no solar energy is charging the battery
                                                        //to free the CPU power for other tasks
            } else {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
        }
    }
}

float SensorSolarPanel::getSolarCurrent(){
    const float ACS712_ZeroCurrentVoltage = 2.5; // 2.5V at 0A
    //const float ACS712_Sensitivity = 0.1875; // 66mV/A 187.5 for 5Amps current sensor
    const float ACS712_Sensitivity = 0.1; // For 20A

    int adValueSolar = 0;

    int finalSamplesSolarCurrent = 0;

    for(int i = 0; i < SENSOR_MESAUREMENT_SAMPLES; i++){
        int adcValueSolarNow = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_SOLAR_CURRENT);

        if(adcValueSolarNow < 0){

        } else {
            adValueSolar += adcValueSolarNow;
            finalSamplesSolarCurrent++;
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    float currentSolar = 0.0f;

    if(finalSamplesSolarCurrent > 0){
        float adcValueSolarCurrent = (float)adValueSolar / (float)finalSamplesSolarCurrent;
        float voltageSolar = adcValueSolarCurrent * 0.0001875; // Convert mV to V
        currentSolar = (voltageSolar - ACS712_ZeroCurrentVoltage) / ACS712_Sensitivity;
    }

    return currentSolar;
}


float SensorSolarPanel::getSolarVoltage(){
    int adValueSolarVoltage = 0;
    int finalSamplesAdcSolarVoltage = 0;

    for(int i = 0; i < SENSOR_MESAUREMENT_SAMPLES; i++){
        int adcValueSolarVoltageNow = ADS1115Controller::instance().readValueBlocking(ADS_INPUT_SOLAR_VOLTAGE);
        if(adcValueSolarVoltageNow  < 0){

        } else {
            adValueSolarVoltage += adcValueSolarVoltageNow;
            finalSamplesAdcSolarVoltage++;
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    float adcValueSolarVoltage = (float)adValueSolarVoltage/(float)finalSamplesAdcSolarVoltage;

    double measuredBatteryVoltageSolar = adcValueSolarVoltage * 0.0001875;
    double scaledVoltageSolar = (measuredBatteryVoltageSolar / 5.0) * 25.0;

    return scaledVoltageSolar;
}