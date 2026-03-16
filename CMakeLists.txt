cmake_minimum_required(VERSION 3.5)

#list(APPEND EXTRA_COMPONENT_DIRS "../../croniot-iot/components")
# Ruta relativa al componente croniot-iot
#set(EXTRA_COMPONENT_DIRS "../../croniot-iot/components/croniot/")


set(EXTRA_COMPONENT_DIRS
    Sensors
    ../../croniot-iot/components/croniot
    ../../croniot-iot/components/espp
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(croniot-watering-system)
