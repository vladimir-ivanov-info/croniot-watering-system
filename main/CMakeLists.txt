# idf_component_register(
#     SRCS "main.cpp"
#     INCLUDE_DIRS "."
# )

idf_component_register(
    SRCS 
        "main.cpp"
        "../SensorsInitializer.cpp"
        "../TasksInitializer.cpp"
        "../Sensors/ADS1115Controller.cpp"
        "../Sensors/Battery/SensorBattery.cpp"
        "../Sensors/SensorCurrentTime.cpp"
        "../Sensors/SensorRamUsage.cpp"
        "../Sensors/SensorSolarPanel.cpp"
        "../Sensors/SensorWiFiInfo.cpp"
        "../Tasks/TaskLedSwitch.cpp"
        "../Tasks/TaskLedTest.cpp"
        "../Tasks/TaskWaterPlants.cpp"
        "../Tasks/TaskWaterPumpSwitch.cpp"
        "../Tasks/TaskLedSlider.cpp"
    INCLUDE_DIRS 
        "."
        ".."
        "../Sensors"
        "../Sensors/Battery"
        "../Tasks"
    REQUIRES 
        croniot  # Tu componente croniot
        driver
        esp_adc
        i2c ads1x15
        #Sensors No pongas REQUIRES Sensors dentro de main, eso es redundante. Si Sensors es un componente, ESP-IDF lo resuelve automáticamente si usas su encabezado.


)

# Incluye los headers del framework si no están bien detectados
#target_include_directories(${COMPONENT_LIB} PRIVATE ../../croniot-iot/include)