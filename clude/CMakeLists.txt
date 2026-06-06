cmake_minimum_required(VERSION 3.20)

# Project definition
project(STM32F407_LED_Blink 
        VERSION 1.0.0
        LANGUAGES CXX C ASM
        DESCRIPTION "STM32F407 LED Blink Application with UART"
)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_FIRMWARE "Build firmware for STM32F407" ON)
option(ENABLE_DEBUG "Enable debug symbols" ON)

# Debug/Release flags
if(ENABLE_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif()

# Compiler flags for embedded development
if(NOT CMAKE_CROSSCOMPILING)
    # Native build (for tests)
    message(STATUS "Building for native platform (unit tests)")
else()
    # ARM Embedded build (for firmware)
    message(STATUS "Building for ARM Cortex-M4 (STM32F407)")
    
    # ARM-specific flags
    set(ARM_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARM_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARM_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARM_FLAGS}")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(CORE_SOURCES
    src/gpio.cpp
    src/uart.cpp
    src/led_manager.cpp
    src/system_init.cpp
    src/gpio_mock.cpp
)

set(TEST_SOURCES
    src/unit_test.cpp
    src/gpio_test.cpp
    src/led_manager_test.cpp
    src/test_runner.cpp
)

# Build Unit Tests
if(BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
    message(STATUS "Building unit tests")
    
    add_executable(test_runner
        ${CORE_SOURCES}
        ${TEST_SOURCES}
    )
    
    target_compile_definitions(test_runner PRIVATE UNIT_TESTING=1)
    target_compile_options(test_runner PRIVATE -fPIC)
    
    # Enable testing
    enable_testing()
    add_test(NAME UnitTests COMMAND test_runner)
    
    message(STATUS "Unit tests enabled - run 'ctest' or './test_runner' to execute")
endif()

# Build Firmware
if(BUILD_FIRMWARE AND CMAKE_CROSSCOMPILING)
    message(STATUS "Building firmware for STM32F407")
    
    # Add firmware executable
    add_executable(stm32f407_firmware.elf
        ${CORE_SOURCES}
        src/main.cpp
    )
    
    # Remove GPIO mock from firmware build
    target_sources(stm32f407_firmware.elf PRIVATE REMOVE src/gpio_mock.cpp)
    
    # Set linker script path (you need to create this)
    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker/stm32f407xx.ld)
    if(EXISTS ${LINKER_SCRIPT})
        set_target_properties(stm32f407_firmware.elf PROPERTIES
            LINK_FLAGS "-T${LINKER_SCRIPT} -Wl,--gc-sections"
        )
    endif()
    
    # Create binary file
    add_custom_command(TARGET stm32f407_firmware.elf
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary stm32f407_firmware.elf stm32f407_firmware.bin
        COMMENT "Creating binary firmware image"
    )
    
    message(STATUS "Firmware build enabled")
endif()

# Add 'make help' target
message(STATUS "")
message(STATUS "Build targets:")
if(BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
    message(STATUS "  test_runner         - Run unit tests")
    message(STATUS "  ctest               - Run tests with CTest")
endif()
if(BUILD_FIRMWARE AND CMAKE_CROSSCOMPILING)
    message(STATUS "  stm32f407_firmware.elf - STM32F407 firmware")
    message(STATUS "  stm32f407_firmware.bin - Binary firmware image")
endif()
message(STATUS "")
message(STATUS "Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Firmware: ${BUILD_FIRMWARE}")
message(STATUS "  Debug Mode: ${ENABLE_DEBUG}")
message(STATUS "")
