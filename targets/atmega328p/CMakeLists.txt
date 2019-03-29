
project(stamping_programmer_atmega328)

include(${ARDUINO_CMAKE_DIR}/atmega328/paths.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/tools.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/defs.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/core.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/libs.cmake)
include(${ARDUINO_CMAKE_DIR}/atmega328/deploy.cmake)

set(ARDUINO_PINS ${ARDUINO_PATH}/hardware/arduino/avr/variants/standard)

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

set(ext_sources ${SDFAT_DIR}/FatLib/FatFile.cpp
    ${SDFAT_DIR}/FatLib/FatFileLFN.cpp
    #${SDFAT_DIR}/FatLib/FatFilePrint.cpp
    #${SDFAT_DIR}/FatLib/FatFileSFN.cpp
    ${SDFAT_DIR}/FatLib/FatVolume.cpp
    #${SDFAT_DIR}/FatLib/FmtNumber.cpp
    #${SDFAT_DIR}/FatLib/StdioStream.cpp
    ${SDFAT_DIR}/FatLib/fstream.cpp
    #${SDFAT_DIR}/FatLib/istream.cpp
    ${SDFAT_DIR}/FatLib/ostream.cpp
    #${SDFAT_DIR}/MinimumSerial.cpp
    ${SDFAT_DIR}/SdCard/SdSpiCard.cpp
    #${SDFAT_DIR}/SdCard/SdSpiCardEX.cpp
    #${SDFAT_DIR}/SdCard/SdioCardEX.cpp
    #${SDFAT_DIR}/SdCard/SdioTeensy.cpp
    #${SDFAT_DIR}/SpiDriver/SdSpiESP8266.cpp
    #${SDFAT_DIR}/SpiDriver/SdSpiParticle.cpp
    #${SDFAT_DIR}/SpiDriver/SdSpiSAM3X.cpp
    #${SDFAT_DIR}/SpiDriver/SdSpiSTM32.cpp
    #${SDFAT_DIR}/SpiDriver/SdSpiTeensy3.cpp
    ${IIC_LCD_SRC}/LiquidCrystal_I2C.cpp
)

add_executable(${PROJECT_NAME} ${product_sources} ${ext_sources} ${arduino_spi_sources} ${arduino_wire_sources} ${atmega328_core_sources})
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
    ${PIN_CHANGE_INT_DIR}
    ${SDFAT_DIR}
    ${ARDUINO_SPI_SRC}
    ${ARDUINO_WIRE_SRC}
    ${IIC_LCD_SRC}
)

upload_atmega328(${PROJECT_NAME} ${PROJECT_NAME})

