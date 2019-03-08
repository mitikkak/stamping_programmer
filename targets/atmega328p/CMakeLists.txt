
project(stamping_programmer_atmega328)

include(${ARDUINO_CMAKE_DIR}/atmega328/paths.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/tools.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/defs.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/core.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/deploy.cmake)

set(ARDUINO_PINS ${ARDUINO_PATH}/hardware/arduino/avr/variants/standard)
#set(ARDUINO_LCD_SRC ${ARDUINO_PATH}/libraries/LiquidCrystal/src/)

add_executable(${PROJECT_NAME} ${product_sources} ${atmega328_core_sources})
target_link_libraries(${PROJECT_NAME} ${atmega328_linker_options})
target_compile_options(
    ${PROJECT_NAME} PRIVATE
    ${atmega328_compiler_set_cpp}
)
target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${ATMEGA328_CORE_SRC} 
    ${ARDUINO_PINS}
    ${SRC_DIR}
    #${GPS_DIR}
    #${SDFAT_DIR}
    #${ARDUINO_LCD_SRC}
    #${IIC_LCD_SRC}
    #${ARDUINO_WIRE_SRC}
)

upload_atmega328(${PROJECT_NAME} ${PROJECT_NAME})

