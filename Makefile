# STM32F407 GPIO and UART Driver Makefile
# Cross-compilation for ARM Cortex-M4

# ========== Tool Chain ==========
ARM_TOOLCHAIN = arm-none-eabi
CC = $(ARM_TOOLCHAIN)-gcc
CXX = $(ARM_TOOLCHAIN)-g++
LD = $(ARM_TOOLCHAIN)-ld
AR = $(ARM_TOOLCHAIN)-ar
OBJCOPY = $(ARM_TOOLCHAIN)-objcopy
OBJDUMP = $(ARM_TOOLCHAIN)-objdump
SIZE = $(ARM_TOOLCHAIN)-size

# ========== Project Directories ==========
SRC_DIR = src
APP_DIR = app
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# ========== Source Files ==========
STARTUP_SOURCES = $(SRC_DIR)/startup.cpp

DRIVER_SOURCES = $(SRC_DIR)/gpio.cpp \
                 $(SRC_DIR)/uart.cpp

APP_SOURCES = $(APP_DIR)/main.cpp

SOURCES = $(STARTUP_SOURCES) $(DRIVER_SOURCES) $(APP_SOURCES)
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/stm32f407_app

# ========== Compiler Flags ==========
# Architecture: Cortex-M4 (STM32F407)
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT_ABI = -mfloat-abi=hard
MCU = $(CPU) $(FPU) $(FLOAT_ABI) -mthumb

# C++ Standard
CXX_STD = -std=c++17

# Optimization and Debug
OPT = -O2
DEBUG = -g3

# Warning Flags
WARNINGS = -Wall -Wextra -Wpedantic -Wfloat-equal \
           -Wshadow -Wpointer-arith -Wcast-align \
           -Wwrite-strings -Wconversion

# Include paths
INCLUDES = -I$(INC_DIR)

# Compiler flags
CXXFLAGS = $(MCU) $(CXX_STD) $(OPT) $(DEBUG) $(WARNINGS) $(INCLUDES) \
           -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections

LDFLAGS = $(MCU) -Tstm32f407.ld -lc -lm -Wl,--gc-sections

# ========== Build Rules ==========
.PHONY: all build clean help size test

all: build

build: $(TARGET).elf $(TARGET).hex $(TARGET).bin
	@echo "Build complete!"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/$(SRC_DIR)
	@mkdir -p $(OBJ_DIR)/$(APP_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(TARGET).elf: $(OBJECTS) | $(BIN_DIR)
	@echo "Linking: $@"
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "ELF file created: $@"

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex
	@echo "HEX file created: $@"

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	@echo "BIN file created: $@"

size: $(TARGET).elf
	$(SIZE) $(TARGET).elf

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

test:
	@echo "Running unit tests..."
	@cd tests && bash run_tests.sh

help:
	@echo "STM32F407 Drivers Makefile"
	@echo "=========================="
	@echo "Targets:"
	@echo "  make build    - Build the firmware (default)"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make size     - Show binary size"
	@echo "  make test     - Build and run unit tests"
	@echo "  make help     - Show this help message"

help:
	@echo "STM32F407 GPIO and UART Driver Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all (default) - Build the project"
	@echo "  build         - Build the project"
	@echo "  clean         - Remove build artifacts"
	@echo "  size          - Display binary size information"
	@echo "  help          - Display this help message"
	@echo ""
	@echo "Generated files:"
	@echo "  build/bin/stm32f407_app.elf - ELF executable"
	@echo "  build/bin/stm32f407_app.hex - Intel HEX format (for programming)"
	@echo "  build/bin/stm32f407_app.bin - Binary format (for programming)"
	@echo ""
	@echo "To program to board:"
	@echo "  Using STLink: st-flash write build/bin/stm32f407_app.bin 0x8000000"
	@echo "  Using OpenOCD: openocd -f board/stm32f4discovery.cfg -c 'program build/bin/stm32f407_app.elf verify reset'"

.PHONY: info
info:
	@echo "Project Configuration:"
	@echo "  Target: STM32F407VG"
	@echo "  CPU: Cortex-M4"
	@echo "  FPU: FPv4-SP-D16"
	@echo "  C++ Standard: C++17"
	@echo "  Optimization: O2"
	@echo "  Compiler: $(CXX)"
	@echo ""
	@echo "Source files:"
	@for src in $(SOURCES); do echo "  - $$src"; done
