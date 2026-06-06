# =============================================================================
# Makefile - STM32F407 Smart Traffic Light Controller (Embedded C++17)
#
# Targets:
#   make            / make firmware  -> cross-compile the .elf/.bin/.hex
#   make test                        -> build & run host unit tests
#   make flash                       -> flash via st-flash (ST-Link)
#   make size                        -> print section sizes
#   make clean                       -> remove build artifacts
#
# The firmware build needs arm-none-eabi-gcc. The host tests build with the
# native g++ and require no hardware or cross toolchain (they exercise the
# platform-independent application and the register drivers against host RAM).
# =============================================================================

# ---- Project ---------------------------------------------------------------
TARGET      := traffic_controller
BUILD_DIR   := build
APP_DIR     := firmware/application
PLAT_DIR    := firmware/platform/stm32
SRC_DIR     := src
STARTUP_DIR := startup
TEST_DIR    := tests
LDSCRIPT    := linker/stm32f407.ld

# Include search paths shared by firmware and tests.
INCLUDES := -I$(APP_DIR) -I$(PLAT_DIR)

# ---- Cross toolchain -------------------------------------------------------
CROSS   := arm-none-eabi-
CC      := $(CROSS)gcc
CXX     := $(CROSS)g++
OBJCOPY := $(CROSS)objcopy
SIZE    := $(CROSS)size

# Cortex-M4F: hardware FPU, Thumb.
MCU_FLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFS      := -DSTM32F407xx

# Common embedded C++ flags: freestanding, no exceptions/RTTI, size-optimised.
CXX_COMMON := -std=c++17 -Wall -Wextra -Wshadow -Wconversion \
              -ffunction-sections -fdata-sections \
              -fno-exceptions -fno-rtti -fno-unwind-tables \
              -fno-use-cxa-atexit

FW_CXXFLAGS := $(MCU_FLAGS) $(DEFS) $(CXX_COMMON) -Os -g $(INCLUDES)
FW_LDFLAGS  := $(MCU_FLAGS) -T$(LDSCRIPT) -Wl,--gc-sections \
               -specs=nano.specs -specs=nosys.specs \
               -Wl,-Map=$(BUILD_DIR)/$(TARGET).map -nostartfiles

# ---- Firmware sources ------------------------------------------------------
FW_SRCS := $(SRC_DIR)/main.cpp \
           $(APP_DIR)/command_parser.cpp \
           $(APP_DIR)/command_processor.cpp \
           $(APP_DIR)/traffic_light.cpp \
           $(PLAT_DIR)/stm32f407_gpio.cpp \
           $(PLAT_DIR)/stm32f407_uart.cpp \
           $(PLAT_DIR)/systick_clock.cpp \
           $(PLAT_DIR)/uart_console.cpp \
           $(STARTUP_DIR)/startup_stm32f407.cpp

FW_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(FW_SRCS))

# ---- Host test build -------------------------------------------------------
HOST_CXX      := g++
HOST_CXXFLAGS := -std=c++17 -Wall -Wextra -Wshadow -g -O0 $(INCLUDES) -I$(TEST_DIR)
TEST_BIN      := $(BUILD_DIR)/run_tests

# Application + register drivers under test, plus the test cases.
TEST_SRCS := $(APP_DIR)/command_parser.cpp \
             $(APP_DIR)/command_processor.cpp \
             $(APP_DIR)/traffic_light.cpp \
             $(PLAT_DIR)/stm32f407_gpio.cpp \
             $(PLAT_DIR)/stm32f407_uart.cpp \
             $(TEST_DIR)/test_ring_buffer.cpp \
             $(TEST_DIR)/test_soft_timer.cpp \
             $(TEST_DIR)/test_line_assembler.cpp \
             $(TEST_DIR)/test_command_parser.cpp \
             $(TEST_DIR)/test_traffic_light.cpp \
             $(TEST_DIR)/test_command_processor.cpp \
             $(TEST_DIR)/test_gpio.cpp \
             $(TEST_DIR)/test_uart.cpp \
             $(TEST_DIR)/test_main.cpp

# ===========================================================================
.PHONY: all firmware test flash size clean

all: firmware

firmware: $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex
	@$(SIZE) $(BUILD_DIR)/$(TARGET).elf

# Compile each firmware translation unit.
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(FW_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(FW_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(FW_OBJS) $(FW_LDFLAGS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) -A -x $<

# Flash to the board (ST-Link). Requires the 'stlink' tools.
flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000

# ---- Host unit tests -------------------------------------------------------
test: $(TEST_BIN)
	@echo "Running unit tests..."
	@./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS)
	@mkdir -p $(BUILD_DIR)
	$(HOST_CXX) $(HOST_CXXFLAGS) $(TEST_SRCS) -o $@

clean:
	rm -rf $(BUILD_DIR)
