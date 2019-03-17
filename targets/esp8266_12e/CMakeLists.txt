project(stamping_programmer_12e)

include(${ARDUINO_CMAKE_DIR}/esp8266/12e.cmake)

set(sources ${product_sources})
set(EXT_SRC_DIR ${SDFAT_DIR}
                ${SPI_ESP8266_DIR}
                ${IIC_LCD_SRC}
                ${WIRE_ESP8266_DIR}
                ${WIFI_ESP8266_DIR}
                ${WEB_SERVER_ESP8266_DIR}
)

set(ext_sources 
    ${IIC_LCD_SRC}/LiquidCrystal_I2C.cpp
    ${WIRE_ESP8266_DIR}/Wire.cpp
    ${esp8266_webserver_sources}
    ${wifi_sources}
)

set(all_sources ${sources} ${ext_sources})
build_targets_esp8266_12e("${all_sources}" "${EXT_SRC_DIR}")