cmake_minimum_required(VERSION 3.10)
project(STM32F407_Tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../include)

# Find Google Test package
find_package(GTest REQUIRED)

# GPIO Tests
add_executable(gpio_test gpio_test.cpp ../src/gpio.cpp)
target_link_libraries(gpio_test GTest::GTest GTest::Main)
target_include_directories(gpio_test PRIVATE ${CMAKE_SOURCE_DIR}/../include)

# UART Tests
add_executable(uart_test uart_test.cpp ../src/uart.cpp ../src/gpio.cpp)
target_link_libraries(uart_test GTest::GTest GTest::Main)
target_include_directories(uart_test PRIVATE ${CMAKE_SOURCE_DIR}/../include)

# Add tests
enable_testing()
add_test(NAME GPIOTests COMMAND gpio_test)
add_test(NAME UARTTests COMMAND uart_test)

# Run all tests
add_custom_target(
    run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS gpio_test uart_test
)
