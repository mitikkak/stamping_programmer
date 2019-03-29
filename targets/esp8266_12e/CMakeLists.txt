project(stamping_programmer_12e)

include(${ARDUINO_CMAKE_DIR}/esp8266/12e.cmake)

set(ESP_ASYNC_TCP_DIR ${PROJECT_ROOT}/ESPAsyncTCP/src)
set(ESP_ASYNC_WEB_SERVER_DIR ${PROJECT_ROOT}/ESPAsyncWebServer/src)
set(esp_async_web_server_sources 
    ${ESP_ASYNC_WEB_SERVER_DIR}/WebServer.cpp
    ${ESP_ASYNC_WEB_SERVER_DIR}/WebRequest.cpp
    ${ESP_ASYNC_WEB_SERVER_DIR}/WebResponses.cpp
)
set(esp_async_tcp_sources
    ${ESP_ASYNC_TCP_DIR}/ESPAsyncTCP.cpp
)

set(product_sources_12e 
        ${SRC_DIR}/Loop_12e.cpp
        ${SRC_DIR}/Setup_12e.cpp
        ${SRC_DIR}/Components_12e.cpp
        ${SRC_DIR}/Program.cpp
        ${SRC_DIR}/SerialPrinter.cpp
        ${SRC_DIR}/Phases.cpp
        ${SRC_DIR}/StringWrapper.cpp
        ${SRC_DIR}/Actuators.cpp
        ${SRC_DIR}/Output.cpp
        ${SRC_DIR}/Buttons.cpp
        ${SRC_DIR}/Verification.cpp
)

set(sources ${product_sources_12e})
set(EXT_SRC_DIR ${SDFAT_DIR}
                ${SPI_ESP8266_DIR}
                ${IIC_LCD_SRC}
                ${WIRE_ESP8266_DIR}
                ${WIFI_ESP8266_DIR}
                #${WEB_SERVER_ESP8266_DIR}
                ${ESP_ASYNC_WEB_SERVER_DIR}
                ${ESP_ASYNC_TCP_DIR}
)

set(ext_sources 
    ${IIC_LCD_SRC}/LiquidCrystal_I2C.cpp
    ${WIRE_ESP8266_DIR}/Wire.cpp
    #${esp8266_webserver_sources}
    ${wifi_sources}
    ${esp_async_web_server_sources}
    ${esp_async_tcp_sources}
)

set(all_sources ${sources} ${ext_sources})
build_targets_esp8266_12e("${all_sources}" "${EXT_SRC_DIR}")