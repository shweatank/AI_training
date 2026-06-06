# Makefile — STM32F407 C++17 LED-chase, bare metal (no HAL).
#
#   make            build .elf/.bin/.hex into build/
#   make flash      flash via st-flash (STLINK)
#   make flash-cube flash via STM32_Programmer_CLI
#   make size       show section sizes
#   make clean      remove build/
# ---------------------------------------------------------------------------

# ---- Toolchain ------------------------------------------------------------
PREFIX  = arm-none-eabi-
CXX     = $(PREFIX)g++
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size

# ---- Project --------------------------------------------------------------
TARGET    = blink
BUILD_DIR = build
LDSCRIPT  = linker/STM32F407VG.ld

SRCS = src/main.cpp \
       src/startup_stm32f407.cpp

OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# ---- CPU / FPU (Cortex-M4F) ----------------------------------------------
CPU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# ---- C++ flags ------------------------------------------------------------
# Freestanding, exceptions/RTTI off — typical for tiny embedded C++.
CXXFLAGS  = $(CPU) -std=c++17
CXXFLAGS += -Wall -Wextra -Wshadow -Wconversion
CXXFLAGS += -Os -ffunction-sections -fdata-sections
CXXFLAGS += -fno-exceptions -fno-rtti -fno-unwind-tables
CXXFLAGS += -fno-use-cxa-atexit -ffreestanding
CXXFLAGS += -Isrc -g3

# ---- Link flags -----------------------------------------------------------
LDFLAGS  = $(CPU) -T$(LDSCRIPT)
LDFLAGS += -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LDFLAGS += -nostartfiles -specs=nano.specs -specs=nosys.specs

# ---- Rules ----------------------------------------------------------------
.PHONY: all clean flash flash-cube size test test-board

all: $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex size

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<

# Flash with open-source stlink tools.
flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000

# Flash with ST's official programmer CLI.
flash-cube: $(BUILD_DIR)/$(TARGET).elf
	STM32_Programmer_CLI -c port=SWD -w $< -rst

# Host-side unit tests (native g++, no board required).
test:
	$(MAKE) -C test

# ---- On-target (hardware-in-the-loop) tests ------------------------------
# Runs the test cases ON the STM32F407 against the real GPIOD peripheral and
# streams each case to this terminal via semihosting. Needs the board attached.
TEST_TGT_ELF  = $(BUILD_DIR)/test_target.elf
TEST_TGT_SRCS = test/target/test_target.cpp src/startup_stm32f407.cpp

$(TEST_TGT_ELF): $(TEST_TGT_SRCS) $(LDSCRIPT)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPU) -T$(LDSCRIPT) -Wl,--gc-sections \
	  -nostartfiles -specs=nano.specs -specs=nosys.specs \
	  $(TEST_TGT_SRCS) -o $@

test-board: $(TEST_TGT_ELF)
	@echo ">>> Flashing on-target tests; streaming results via semihosting..."
	@echo ">>> (each test case prints below as it runs on the board)"
	-timeout 30 openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
	  -c "gdb_port disabled" -c "tcl_port disabled" -c "telnet_port disabled" \
	  -c "init" -c "reset halt" \
	  -c "flash write_image erase $(TEST_TGT_ELF)" \
	  -c "reset halt" -c "arm semihosting enable" -c "resume"

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C test clean
