# ============================================================================
#  STM32F407 LED-blink firmware - embedded C++17
#  Cross-compiles for Cortex-M4F and flashes via OpenOCD / ST-Link.
# ============================================================================

# ---- Toolchain ----
PREFIX  = arm-none-eabi-
CXX     = $(PREFIX)g++
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size

# ---- Project layout ----
SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build
TARGET    = $(BUILD_DIR)/led_blink
LDSCRIPT  = stm32f407.ld

SRCS = $(SRC_DIR)/startup.cpp \
       $(SRC_DIR)/gpio.cpp \
       $(SRC_DIR)/main.cpp
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# ---- CPU / FPU ----
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# ---- Flags ----
CXXFLAGS = $(MCU) -std=c++17 -O2 -g3 \
           -ffunction-sections -fdata-sections \
           -fno-exceptions -fno-rtti -fno-use-cxa-atexit \
           -Wall -Wextra -Wshadow -Wconversion \
           -I$(INC_DIR)

LDFLAGS  = $(MCU) -T$(LDSCRIPT) -nostartfiles \
           --specs=nano.specs --specs=nosys.specs \
           -Wl,--gc-sections -Wl,-Map=$(TARGET).map

# ---- OpenOCD (ST-Link V2.1 on STM32F4) ----
OPENOCD       = openocd
OPENOCD_CFG   = -f interface/stlink.cfg -f target/stm32f4x.cfg

# ============================================================================
.PHONY: all build size flash erase clean test help

all: build

build: $(TARGET).elf $(TARGET).hex $(TARGET).bin size

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "CXX  $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	@echo "LD   $@"
	@$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(TARGET).hex: $(TARGET).elf
	@$(OBJCOPY) -O ihex $< $@

$(TARGET).bin: $(TARGET).elf
	@$(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	@$(SIZE) $(TARGET).elf

# Flash the .elf and reset/run.
flash: $(TARGET).elf
	$(OPENOCD) $(OPENOCD_CFG) \
	  -c "program $(TARGET).elf verify reset exit"

# Mass-erase the chip.
erase:
	$(OPENOCD) $(OPENOCD_CFG) -c "init; reset halt; stm32f4x mass_erase 0; exit"

# Build and run host unit tests.
test:
	@$(MAKE) -C tests test

clean:
	@rm -rf $(BUILD_DIR)
	@$(MAKE) -C tests clean

help:
	@echo "Targets:"
	@echo "  make build   - cross-compile firmware (elf/hex/bin) [default]"
	@echo "  make flash   - flash firmware to STM32F407 via OpenOCD/ST-Link"
	@echo "  make erase   - mass-erase the chip"
	@echo "  make size    - show firmware size"
	@echo "  make test    - build & run host unit tests (GoogleTest)"
	@echo "  make clean   - remove all build artifacts"
