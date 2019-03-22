
cmake_minimum_required(VERSION 3.1)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ARDUINO_CMAKE_DIR ${PROJECT_ROOT}/arduino_cmake)
set(SRC_DIR ${PROJECT_ROOT}/src)
set(PIN_CHANGE_INT_DIR ${PROJECT_ROOT}/PinChangeInt )
set(SDFAT_DIR ${PROJECT_ROOT}/SdFat/src)
set(IIC_LCD_SRC ${PROJECT_ROOT}/LiquidCrystal_I2C)

set(product_sources ${SRC_DIR}/Loop.cpp
        ${SRC_DIR}/Setup.cpp
        ${SRC_DIR}/Components.cpp
        ${SRC_DIR}/Program.cpp
        ${SRC_DIR}/SerialPrinter.cpp
        ${SRC_DIR}/Phases.cpp
        ${SRC_DIR}/StringWrapper.cpp
        ${SRC_DIR}/Actuators.cpp
        ${SRC_DIR}/Output.cpp
        ${SRC_DIR}/Buttons.cpp
        ${SRC_DIR}/Verification.cpp
)

add_subdirectory(${PROJECT_ROOT}/googletest)
add_subdirectory(${PROJECT_ROOT}/targets/atmega328p)
add_subdirectory(${ARDUINO_CMAKE_DIR}/esp8266/core)
add_subdirectory(${PROJECT_ROOT}/targets/esp8266_12e)
add_subdirectory(${PROJECT_ROOT}/targets/ut)