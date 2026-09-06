cmake_minimum_required(VERSION 3.14)
project(croniot_watering_system_host_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Host-native tests: plain g++/clang++, zero ESP-IDF/FreeRTOS dependency.
# Only compiles the pure-logic .cpp files extracted from Sensors/ and Tasks/
# (never the ESP-IDF-coupled Sensor*.cpp / Task*.cpp themselves).

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(host_tests
    ${REPO_ROOT}/Sensors/Battery/SensorBatteryLogic.cpp
    ${REPO_ROOT}/Sensors/SensorSolarPanelLogic.cpp
    ${REPO_ROOT}/Sensors/SensorWiFiInfoLogic.cpp
    ${REPO_ROOT}/Sensors/SensorCurrentTimeLogic.cpp
    ${REPO_ROOT}/Tasks/TaskWaterPlantsLogic.cpp

    SensorBatteryLogicTest.cpp
    SensorSolarPanelLogicTest.cpp
    SensorWiFiInfoLogicTest.cpp
    SensorCurrentTimeLogicTest.cpp
    TaskWaterPlantsLogicTest.cpp
    WaterSequenceControllerTest.cpp
    WaterTaskInteractionHazardTest.cpp
)

target_include_directories(host_tests PRIVATE
    ${REPO_ROOT}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(host_tests PRIVATE gtest_main gmock)

include(GoogleTest)
gtest_discover_tests(host_tests)
