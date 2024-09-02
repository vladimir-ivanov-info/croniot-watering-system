#include "SensorBattery2.h"

#define BATTERY_CURRENT_BIT (1 << 0)
#define BATTERY_VOLTAGE_BIT (1 << 1)

SensorBattery2::SensorBattery2() : continueTask(true), taskHandle(NULL) {

    batteryCurrentValueQueue = xQueueCreate(1, sizeof(int16_t));
    sensorVoltageQueue = xQueueCreate(1, sizeof(int16_t));

    solarCurrentQueue = xQueueCreate(1, sizeof(int16_t));

    eventGroup = xEventGroupCreate();

    if (batteryCurrentValueQueue == NULL || eventGroup == NULL) {
        Serial.println("Failed to create queue or event group");
        return;
    }

    if (sensorVoltageQueue == NULL || eventGroup == NULL) {
        Serial.println("Failed to create queue or event group");
        return;
    }

    if (solarCurrentQueue == NULL || eventGroup == NULL) {
        Serial.println("Failed to create queue or event group for solar");
        return;
    }

    Wire.begin();

    ADS = ADS1115(address);
    Serial.print(address, HEX);
    Serial.print("  ");
    Serial.println(ADS.begin() ? "ADS connected" : "not connected");

    //  0 = slow   4 = medium   7 = fast, but more noise
    ADS.setDataRate(7);
    // ADS.setGain(1); //+- 4.096 V
    ADS.setGain(0); //± 6.144 V

    if (ADS.isConnected()) {
        ADS.requestADC(ADS_INPUT_BATTERY_CURRENT);
        ADS.requestADC(ADS_INPUT_BATTERY_VOLTAGE);
        ADS.requestADC(ADS_INPUT_SOLAR_VOLTAGE);
        ADS.requestADC(ADS_INPUT_SOLAR_CURRENT);
    }
}

void SensorBattery2::run() {
    xTaskCreatePinnedToCore(
        taskFunction,      // Function to run the task
        "SensorBattery",   // Task name
        11000,             // Stack size (words)
        this,              // Task parameter (pass the instance of the singleton)
        1,                 // Task priority
        &taskHandle,       // Task handle
        0                  // Core to run the task (0 or 1)
    );
}

void SensorBattery2::stop() {
    continueTask = false;
}

float SensorBattery2::voltsToPercentage(float voltage) { 
    float maxVoltage = 13.3;
    float minVoltage = 9.7;

    float minPercentage = 0.0;
    float maxPercentage = 100.0;

    float slope = (maxPercentage - minPercentage) / (maxVoltage - minVoltage);
    float percentage = slope * (voltage - minVoltage) + minPercentage;

    return percentage;
}

void SensorBattery2::taskFunction(void *param) {
    SensorBattery2 *self = (SensorBattery2 *)param;

    xTaskCreate(
        measureBatteryCurrent,    // Task function
        "Measure battery current",        // Task name
        2048,                     // Stack size
        self,                     // Task input parameter (pass the instance of the object)
        1,                        // Priority
        &self->batteryCurrentTaskHandle  // Task handle
    );
    vTaskSuspend(self->batteryCurrentTaskHandle);


    while (true) {
        xEventGroupClearBits(self->eventGroup, BATTERY_CURRENT_BIT/* | BATTERY_VOLTAGE_BIT*/);
        vTaskResume(self->batteryCurrentTaskHandle);
        xEventGroupWaitBits(self->eventGroup, BATTERY_CURRENT_BIT /*| BATTERY_VOLTAGE_BIT*/, pdTRUE, pdTRUE, portMAX_DELAY);

        int16_t sensorValueCurrent = 99;
        if (xQueueReceive(self->batteryCurrentValueQueue, &sensorValueCurrent, portMAX_DELAY) == pdPASS) {
            Serial.print("Current ADC value: "); Serial.println(sensorValueCurrent);
        }

        int16_t sensorValueVoltage = 99;
        if (xQueueReceive(self->sensorVoltageQueue, &sensorValueVoltage, portMAX_DELAY) == pdPASS) {
            Serial.print("Voltage ADC value: "); Serial.println(sensorValueVoltage);
        }

        double current = calculateCurrent(sensorValueCurrent);
        double voltage = calculateVoltage(sensorValueVoltage);
        double power = current*voltage;
        double percentage = voltsToPercentage(voltage);

        Serial.print("Battery percentage: "); Serial.print(percentage); Serial.println(" %");
        Serial.print("Battery power: "); Serial.print(power); Serial.println(" W");

        //SOLAR
        int16_t sensorSolarCurrent = 99;
        if (xQueueReceive(self->solarCurrentQueue, &sensorSolarCurrent, portMAX_DELAY) == pdPASS) {
            Serial.print("Solar current ADC value: "); Serial.println(sensorSolarCurrent);// Process the received sensor value
        }

        double solarCurrent = calculateCurrent(sensorSolarCurrent);
        double solarPower = solarCurrent*voltage;
        Serial.print("Solar current: "); Serial.print(solarCurrent); Serial.print(" A | ");
        Serial.print("power: "); Serial.print(solarPower); Serial.println(" W");
        String solarPowerStr = String(solarPower);

        //MQTT
        vTaskDelay(100 / portTICK_PERIOD_MS);

        String batteryPowerStr = String(power);
        int sensorBatteryPercentage = static_cast<int>(SENSOR_BATTERY_PERCENTAGE);
        String topicBatteryPercentage = "esp32id_outcoming/sensor_data/" + String(sensorBatteryPercentage);
        int sensorBatteryPowerConsumption = static_cast<int>(SENSOR_BATTERY_POWER_CONSUMPTION);
        String batteryPercentageStr = String(percentage);
        MQTTManager::instance().publish(topicBatteryPercentage.c_str(), batteryPercentageStr.c_str());

        String topicBatteryPowerConsumption = "esp32id_outcoming/sensor_data/" + String(sensorBatteryPowerConsumption);
        MQTTManager::instance().publish(topicBatteryPowerConsumption.c_str(), batteryPowerStr.c_str());

        int sensorSolarPower = static_cast<int>(SENSOR_SOLAR_POWER);
        String topicSensorSolarPower = "esp32id_outcoming/sensor_data/" + String(sensorSolarPower);
        MQTTManager::instance().publish(topicSensorSolarPower.c_str(), solarPowerStr.c_str());
    }

    Serial.println("SensorBattery task stopped");
    vTaskDelete(NULL);
}


void SensorBattery2::measureBatteryCurrent(void *param) {
    Serial.println("Task current sensor started");
    SensorBattery2 *self = (SensorBattery2 *)param;

    while (true) {
        int analogValueSum = 0;  // Example ADC read and conversion
        int analogValueSolarSum = 0;
        int samples = 20;

        int analogValueSumVoltage = 0;
        int analogValueSumSolarVoltage = 0;
        bool isCurrentTurn = true;

        int delayMilliseconds = 50;

        for(int i = 0; i < samples; i++){
            int analogValue = measureADSPin(self->ADS, self->ADS_INPUT_BATTERY_CURRENT);
            analogValueSum += analogValue;
            vTaskDelay(delayMilliseconds / portTICK_PERIOD_MS);
            int analogValueVoltage = measureADSPin(self->ADS, self->ADS_INPUT_BATTERY_VOLTAGE);
            analogValueSumVoltage += analogValueVoltage;
            vTaskDelay(delayMilliseconds / portTICK_PERIOD_MS);

            int analogValueSolarCurrent = measureADSPin(self->ADS, self->ADS_INPUT_SOLAR_CURRENT);
            analogValueSolarSum += analogValueSolarCurrent;
            vTaskDelay(delayMilliseconds / portTICK_PERIOD_MS);
        }

        int16_t averageValue = analogValueSum / samples;
        int16_t averageValueVoltage = analogValueSumVoltage / samples;

        int16_t averageValueCurrentSolar = analogValueSolarSum / samples;
        int16_t averageValueSolarVoltage = analogValueSumSolarVoltage / samples;

        // Send the sensor value to the queue
        if (xQueueSend(self->batteryCurrentValueQueue, &averageValue, portMAX_DELAY) != pdPASS) {
            Serial.println("Failed to send sensor value to queue");
        } else {
            Serial.print("Sent current sensor value: ");
            Serial.println(averageValue);
        }

        if (xQueueSend(self->sensorVoltageQueue, &averageValueVoltage, portMAX_DELAY) != pdPASS) {
            Serial.println("Failed to send sensor value to queue");
        } else {
            Serial.print("Sent current sensor value: ");
            Serial.println(averageValueVoltage);
        }

        if (xQueueSend(self->solarCurrentQueue, &averageValueCurrentSolar, portMAX_DELAY) != pdPASS) {
            Serial.println("Failed to send sensor value to solar queue");
        } else {
            Serial.print("Sent solar current sensor value: ");
            Serial.println(averageValue);
        }

        xEventGroupSetBits(self->eventGroup, BATTERY_CURRENT_BIT);
        
        vTaskSuspend(NULL); // Suspend the task until it is resumed again
    }
}

int16_t SensorBattery2::measureADSPin(ADS1115 ADS, int pin){
    if (ADS.isConnected() && ADS.isBusy()){
        return -1;
    } else {
        ADS.requestADC(pin);
        int16_t adcValue = ADS.readADC(pin);
        float voltage = ADS.toVoltage(adcValue);
        return adcValue;
    }
}

String SensorBattery2::analogValueToBatteryCurrent(int16_t analogValue){

    return "";
}

double SensorBattery2::calculateCurrent(int adcValue){
    //TODO
    //max adcvalue corresponding to 5V = 5*32768/6.114 = 26798
    //so 2.5V corresponds to adc = 13399
    //TODO this method should receive a correction percentage so that values that deviate for example 1% from the center (13399) return 0 Amps
    //1% = 134
    //below = 13265
    //above = 13533

    Serial.print("Calculating current for ADC: "); Serial.print(adcValue); Serial.println();
   // const float ACS712_Sensitivity = 0.1875; // 66mV/A 187.5 for 5A
    const float ACS712_Sensitivity = 0.1f; // 100 mV/A for 20A

    const float ACS712_ZeroCurrentVoltage = 2.5f;

    //float voltage = adcValue * ACS712_Sensitivity / 1000;
    float voltage = (adcValue / 32768.0f) * 6.144f;
    Serial.print("Calculating current for ADC voltage: "); Serial.print(voltage, 4); Serial.println();


    float correction = 0.9933f;

    float finalVoltage = (5.0f - voltage) * correction;


    Serial.print("Battery Ammeter: "); Serial.print(finalVoltage); Serial.print(" V");
    float current = (finalVoltage - ACS712_ZeroCurrentVoltage) / ACS712_Sensitivity;

    Serial.print(" | Current: "); 
    Serial.print(current, 3);
    Serial.println(" A");

    //Noise correction
    if(fabs(current) < 0.1){ //0.1 = noise factor
        current = 0.0f;
    }

    return current;
}

double SensorBattery2::calculateVoltage(int adcValue){
    float voltage0 = (adcValue / 32768.0) * 6.144;
    float measuredVoltage = (voltage0 / 5.0) * 25.0;  // Scale from 0-5V to 0-25V
    Serial.print("Battery voltage: "); Serial.print(measuredVoltage); Serial.println(" V");
    return measuredVoltage;
}