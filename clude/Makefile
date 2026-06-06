# Makefile for STM32F407 LED Blink Application
# Supports both native unit tests and ARM firmware build

.PHONY: all help clean test firmware native

# Project settings
PROJECT_NAME = stm32f407_led_blink
VERSION = 1.0.0

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
FIRMWARE_BUILD = $(BUILD_DIR)/firmware
TEST_BUILD = $(BUILD_DIR)/test

# Compiler settings
CXX = g++
ARM_CXX = arm-none-eabi-g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -I$(INC_DIR)
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2

# Default build type
BUILD_TYPE ?= Debug

ifeq ($(BUILD_TYPE), Debug)
    CXXFLAGS += $(DEBUG_FLAGS)
else
    CXXFLAGS += $(RELEASE_FLAGS)
endif

# Source files
CORE_SOURCES = $(SRC_DIR)/gpio.cpp \
               $(SRC_DIR)/uart.cpp \
               $(SRC_DIR)/led_manager.cpp \
               $(SRC_DIR)/system_init.cpp \
               $(SRC_DIR)/gpio_mock.cpp

TEST_SOURCES = $(SRC_DIR)/unit_test.cpp \
               $(SRC_DIR)/gpio_test.cpp \
               $(SRC_DIR)/led_manager_test.cpp \
               $(SRC_DIR)/test_runner.cpp

FIRMWARE_SOURCES = $(SRC_DIR)/gpio.cpp \
                   $(SRC_DIR)/uart.cpp \
                   $(SRC_DIR)/led_manager.cpp \
                   $(SRC_DIR)/system_init.cpp \
                   $(SRC_DIR)/main.cpp

# Object files
TEST_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(TEST_BUILD)/%.o, $(CORE_SOURCES) $(TEST_SOURCES))
FIRMWARE_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(FIRMWARE_BUILD)/%.o, $(FIRMWARE_SOURCES))

# Output files
TEST_EXE = $(TEST_BUILD)/test_runner
FIRMWARE_ELF = $(FIRMWARE_BUILD)/stm32f407_firmware.elf
FIRMWARE_BIN = $(FIRMWARE_BUILD)/stm32f407_firmware.bin

# Help target
help:
	@echo "STM32F407 LED Blink Application - Makefile"
	@echo "=========================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build everything (default)"
	@echo "  test          - Build and run unit tests"
	@echo "  native        - Build unit tests (native)"
	@echo "  firmware      - Build firmware for STM32F407"
	@echo "  clean         - Clean all build artifacts"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Options:"
	@echo "  BUILD_TYPE=Debug|Release  (default: Debug)"
	@echo ""
	@echo "Examples:"
	@echo "  make test                 - Build and run tests"
	@echo "  make BUILD_TYPE=Release   - Release build"
	@echo "  make firmware             - Build firmware"
	@echo ""

# Default target
all: test

# Native unit tests
native: $(TEST_EXE)

$(TEST_EXE): $(TEST_OBJECTS)
	@mkdir -p $(TEST_BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Built: $@"

$(TEST_BUILD)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(TEST_BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	@echo "✓ Compiled: $<"

# Run tests
test: native
	@echo ""
	@echo "=========================================="
	@echo "Running Unit Tests"
	@echo "=========================================="
	@echo ""
	@./$(TEST_EXE)
	@echo ""

# Firmware build
firmware: $(FIRMWARE_BIN)

$(FIRMWARE_ELF): $(FIRMWARE_OBJECTS)
	@mkdir -p $(FIRMWARE_BUILD)
	$(ARM_CXX) -mcpu=cortex-m4 -mthumb -o $@ $^ 
	@echo "✓ Linked: $@"
	@arm-none-eabi-size $@

$(FIRMWARE_BIN): $(FIRMWARE_ELF)
	arm-none-eabi-objcopy -O binary $< $@
	@echo "✓ Binary: $@"

$(FIRMWARE_BUILD)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(FIRMWARE_BUILD)
	$(ARM_CXX) -mcpu=cortex-m4 -mthumb -std=c++17 -I$(INC_DIR) -c -o $@ $<
	@echo "✓ Compiled (ARM): $<"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned build artifacts"

# Info target
info:
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "C++ Standard: C++17"
	@echo "Test Framework: Custom lightweight unit test framework"
	@echo "Compiler: $(CXX)"
	@echo "ARM Compiler: $(ARM_CXX)"
	@echo ""
	@echo "Directories:"
	@echo "  Source: $(SRC_DIR)"
	@echo "  Include: $(INC_DIR)"
	@echo "  Build: $(BUILD_DIR)"
	@echo ""

.PHONY: all help clean test firmware native info
