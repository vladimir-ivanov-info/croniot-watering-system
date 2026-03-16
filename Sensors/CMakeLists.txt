idf_component_register(
    SRCS "ADS1115Controller.cpp"
    INCLUDE_DIRS "."
                 #"../../../croniot-iot/components/espp/lib"
    REQUIRES i2c ads1x15
)
