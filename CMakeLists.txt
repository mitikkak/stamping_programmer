
cmake_minimum_required(VERSION 3.1)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ARDUINO_CMAKE_DIR ${PROJECT_ROOT}/arduino_cmake)
set(SRC_DIR ${PROJECT_ROOT}/src)

add_subdirectory(${PROJECT_ROOT}/googletest)
add_subdirectory(${PROJECT_ROOT}/targets/atmega328p)
add_subdirectory(${PROJECT_ROOT}/targets/ut)