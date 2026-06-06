# ============================================================================
#  Host unit-test build for the GPIO driver (GoogleTest).
#  Compiles the SAME src/gpio.cpp that ships to the target, linked against a
#  fake register block in host RAM. No ARM toolchain needed for this target.
# ============================================================================

CXX      ?= g++
CXXSTD    = -std=c++17
CXXFLAGS  = $(CXXSTD) -O0 -g -Wall -Wextra -I../include
LDFLAGS   = -lgtest -lgtest_main -lpthread

BUILD_DIR = build
RUNNER    = $(BUILD_DIR)/gpio_test

SRCS = gpio_test.cpp ../src/gpio.cpp

.PHONY: all test clean

all: test

$(RUNNER): $(SRCS) ../include/gpio.hpp ../include/stm32f407_registers.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(LDFLAGS)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

test: $(RUNNER)
	@echo "=== Running GPIO unit tests ==="
	./$(RUNNER) --gtest_color=yes

clean:
	@rm -rf $(BUILD_DIR)
