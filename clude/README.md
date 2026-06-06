# STM32F407 LED Blink Application with UART

A professional C++17 embedded systems application for the STM32F407 Discovery board featuring LED blinking control, UART communication, and comprehensive unit tests.

## Overview

This project demonstrates best practices in embedded C++ development:

- **C++17** modern C++ features
- **Hardware Abstraction Layer (HAL)** for GPIO and UART
- **LED Manager** for control of all 4 LEDs on the board
- **UART Driver** for serial communication (115200 baud)
- **Comprehensive Unit Tests** with mock objects
- **CMakeLists.txt** for cross-platform building
- **Makefile** for traditional make-based builds

## Project Structure

```
.
├── include/                 # Header files
│   ├── stm32f407xx.h       # Hardware register definitions
│   ├── gpio.h              # GPIO driver interface
│   ├── uart.h              # UART driver interface
│   ├── led_manager.h       # LED management interface
│   ├── system_init.h       # System initialization
│   ├── gpio_mock.h         # GPIO mock for testing
│   ├── unit_test.h         # Unit test framework
│   ├── gpio_test.h         # GPIO tests
│   └── led_manager_test.h  # LED Manager tests
├── src/                     # Implementation files
│   ├── gpio.cpp            # GPIO driver implementation
│   ├── uart.cpp            # UART driver implementation
│   ├── led_manager.cpp     # LED manager implementation
│   ├── system_init.cpp     # System initialization
│   ├── gpio_mock.cpp       # GPIO mock implementation
│   ├── unit_test.cpp       # Unit test framework
│   ├── gpio_test.cpp       # GPIO unit tests
│   ├── led_manager_test.cpp# LED manager unit tests
│   ├── test_runner.cpp     # Test suite runner
│   └── main.cpp            # Main application
├── linker/
│   └── stm32f407xx.ld      # Linker script for STM32F407
├── CMakeLists.txt          # CMake build configuration
├── Makefile                # Traditional Makefile
└── README.md               # This file

```

## STM32F407 Discovery Board LED Configuration

The board has 4 LEDs:

- **Green LED**:  PD12
- **Orange LED**: PD13
- **Red LED**:    PD14
- **Blue LED**:   PD15

## Features

### GPIO Driver (`gpio.h`)
- Pin configuration (mode, speed, pull-up/pull-down)
- Set/reset individual pins
- Toggle pins
- Read pin state
- Atomic multi-pin operations
- Comprehensive error handling

### UART Driver (`uart.h`)
- **Baud Rate**: 115200
- Send single character, string, or raw data
- Receive character with timeout
- Non-blocking data availability check
- Hardware initialization and configuration

### LED Manager (`led_manager.h`)
- Control individual LEDs by color
- Sequential LED blinking
- Simultaneous LED blinking
- LED status reporting via UART
- Extensible design for additional effects

### System Initialization (`system_init.h`)
- Clock configuration (168 MHz from PLL)
- Peripheral clock enablement
- SysTick timer for system timing
- Flash memory configuration

## Building and Running

### Prerequisites

For unit tests (native):
```bash
g++ (or clang++)
CMake >= 3.20 (optional)
```

For firmware (cross-compilation):
```bash
arm-none-eabi-gcc/g++
arm-none-eabi-objcopy
STM32CubeProgrammer or similar flash tool
```

### Using CMake (Recommended for unit tests)

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake -DBUILD_TESTS=ON -DBUILD_FIRMWARE=OFF ..

# Build
cmake --build .

# Run tests
ctest --verbose
# Or
./test_runner
```

### Using Makefile

```bash
# Build and run unit tests
make test

# Clean build artifacts
make clean

# Build firmware (requires ARM toolchain)
make firmware

# Get help
make help
```

### Building Individual Tests

```bash
# Just build tests without running
make native

# Run tests explicitly
./build/test/test_runner
```

## Test Coverage

The project includes comprehensive unit tests for:

### GPIO Tests (12 test cases)
- ✓ GPIO initialization
- ✓ Pin configuration (mode, speed, pull)
- ✓ Set pin HIGH/LOW
- ✓ Toggle pin
- ✓ Read pin state
- ✓ Multi-pin operations
- ✓ Invalid pin handling
- ✓ All GPIO modes (INPUT, OUTPUT, ALTERNATE, ANALOG)
- ✓ All speed settings (SLOW, MEDIUM, FAST, VERY_FAST)
- ✓ All pull configurations (NO_PULL, PULL_UP, PULL_DOWN)

### LED Manager Tests (10 test cases)
- ✓ LED Manager initialization
- ✓ Turn LED on/off
- ✓ Toggle LED
- ✓ Single LED blink
- ✓ Sequential all-LED blink
- ✓ Simultaneous all-LED blink
- ✓ Get LED pin mapping
- ✓ Get LED count
- ✓ All LED color control
- ✓ UART status reporting

## Running Tests

```bash
# Run all tests with verbose output
./build/test/test_runner

# Sample output:
# ========================================
# Running GPIO Driver Tests
# ========================================
# [TEST] GPIO Initialization... PASSED
# [TEST] GPIO Pin Configuration... PASSED
# [TEST] Set GPIO Pin HIGH... PASSED
# ...
# Test Summary:
#   Total:  22
#   Passed: 22
#   Failed: 0
#   Rate:   100.0%
```

## Application Flow

```cpp
1. Initialize system clocks (168 MHz)
2. Initialize GPIO and UART drivers
3. Initialize LED Manager
4. Main loop:
   - Blink all LEDs sequentially
   - Blink all LEDs simultaneously
   - Individual LED control
   - Send status via UART
   - Repeat every 2 seconds
```

## UART Communication Format

Status messages are sent as:
```
LED: [COLOR] - [ON/OFF]

Examples:
LED: GREEN - ON
LED: GREEN - OFF
LED: ORANGE - ON
```

Startup message:
```
========================================
STM32F407 LED Blink Application v1.0
========================================
```

## Design Patterns Used

### Mock Objects
The `GPIOMock` class allows testing GPIO functionality without hardware, using mock register structures.

### Singleton Pattern (Static Methods)
GPIO, UART, and LEDManager use static methods for global access without explicit object instantiation.

### Hardware Abstraction Layer
Separate driver implementations (GPIO, UART) from application logic (LED Manager) for maintainability.

### Test Framework
Custom lightweight unit test framework with:
- Setup/teardown capabilities
- Assertion macros
- Test result reporting
- Summary statistics

## Compiler Flags

```
-std=c++17          # C++ 17 standard
-Wall -Wextra       # All common warnings
-Wpedantic          # Pedantic warnings
-g -O0              # Debug: symbols, no optimization
-O2                 # Release: optimization level 2
```

For ARM firmware:
```
-mcpu=cortex-m4     # Cortex-M4 processor
-mthumb             # Thumb instruction set
-mfpu=fpv4-sp-d16   # Single-precision FPU
-mfloat-abi=hard    # Hardware floating point
```

## C++17 Features Used

- `constexpr` for compile-time constants
- `std::array` for fixed-size arrays
- `enum class` for type-safe enumerations
- `inline constexpr` for constexpr pointers
- Range-based for loops
- Auto type deduction
- nullptr

## Performance Characteristics

- **GPIO Operations**: O(1) - single register operations
- **UART Send**: O(n) where n = number of bytes
- **LED Blink**: Configurable delay (default 100ms)
- **Memory Overhead**: ~4KB for driver structures

## Future Enhancements

- [ ] PWM LED brightness control
- [ ] UART interrupt-driven communication
- [ ] SD card support via SPI
- [ ] Temperature sensor integration (I2C)
- [ ] Real-time operating system (FreeRTOS)
- [ ] Low-power sleep modes
- [ ] Error recovery mechanisms
- [ ] CAN bus driver

## Troubleshooting

### Tests won't compile
- Ensure C++17 compiler support: `g++ -std=c++17 --version`
- Check all include paths: `include/` directory must be in search path

### Firmware won't compile
- Install ARM toolchain: `sudo apt install arm-none-eabi-gcc`
- Check linker script path in CMakeLists.txt

### Tests fail with "assert not working"
- Ensure mock objects are properly initialized
- Check GPIO mock reset between tests

## References

- STM32F407 Datasheet
- ARM Cortex-M4 Reference Manual
- C++17 Standard Features
- Modern CMake Practices

## License

Educational and embedded systems development

## Author

Embedded C++ Expert
Version 1.0.0
