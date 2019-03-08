project(ut)

set(STUBS_DIR ${ARDUINO_CMAKE_DIR}/ut/stubs)
set(TEST_SRC_DIR ${PROJECT_ROOT}/tests)
set(test_sources 
             ${ARDUINO_CMAKE_DIR}/ut/Runner.cpp
             ${TEST_SRC_DIR}/TestCreateProgram.cpp
             ${TEST_SRC_DIR}/TestRunProgram.cpp
)

set(stub_sources 
		${STUBS_DIR}/Print.cpp
		${STUBS_DIR}/Arduino.cpp
		${STUBS_DIR}/PinEvents.cpp
)

add_executable(${PROJECT_NAME} 
                                ${test_sources}
                                ${stub_sources}
                                ${product_sources}
)

set(GTEST_LIBRARIES gtest gmock)
target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} pthread)
target_compile_options(
    ${PROJECT_NAME} PRIVATE
    -std=gnu++11
)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE 
      ${PROJECT_ROOT}/googletest/googletest/include 
      ${STUBS_DIR} 
      ${SRC_DIR} 
      ${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino
      #${ACCELEROMETER_DIR} 
      #${IIC_LCD_SRC}
)

target_compile_definitions(
    ${PROJECT_NAME} PRIVATE
    UNIT_TEST
    ARDUINO=10800
)