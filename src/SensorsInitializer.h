#ifndef SENSORSINITIALIZER_H
#define SENSORSINITIALIZER_H

#include "Sensors/SensorDefs.h"

#include "Sensors/SensorsController.h"

#include "Sensors/SensorWiFiStrength.h"
#include "Sensors/Battery/SensorBattery.h"
#include "Sensors/SensorSolarPanel.h"
#include "Sensors/SensorRamUsage.h"
#include "Sensors/SensorCurrentTime.h"

class SensorsInitializer {

    public:
        static void initSensors();

    private:
        static void registerSensorTypes();
        static void registerSensors();


};

#endif