# STM32F407 GPIO and UART Driver Library

A modern C++17 embedded driver library for STM32F407 microcontroller featuring GPIO and UART modules with a complete example application.

## Project Structure

```
STM32F407_Drivers/
├── include/              # Header files
│   ├── gpio.hpp         # GPIO driver interface
│   ├── uart.hpp         # UART driver interface
│   └── stm32f407_registers.hpp  # Register definitions and memory layout
├── src/                 # Implementation files
│   ├── gpio.cpp         # GPIO driver implementation
│   └── uart.cpp         # UART driver implementation
├── app/                 # Application example
│   └── main.cpp         # Main application with GPIO and UART demo
├── build/               # Build artifacts (generated)
├── Makefile             # Build configuration
└── README.md            # This file
```

## Features

### GPIO Driver (`gpio.hpp` / `gpio.cpp`)
- **Port Support**: GPIOA through GPIOI
- **Pin Operations**: 
  - Initialize pins with mode, speed, and pull configuration
  - Read/write individual pins
  - Toggle pins
  - Read/write entire port
- **Alternate Function**: Support for peripheral remapping (AF0-AF15)
- **Auto Clock Enable**: Automatic RCC configuration for port clock

### UART Driver (`uart.hpp` / `uart.cpp`)
- **Port Support**: UART1-6 (USART1-6)
- **Features**:
  - Configurable baud rate (tested at 115200)
  - 8/9-bit data word length
  - Single/double stop bits
  - Parity control (odd, even, none)
- **Operations**:
  - Send/receive single bytes
  - Send strings
  - Check data availability
  - Check transmitter status
- **Auto Pin Configuration**: Automatic GPIO alternate function setup

### Example Application (`main.cpp`)
The application demonstrates:
- LED control on GPIOD (pins 12-15)
- Button input on GPIOA (pin 0)
- UART serial communication at 115200 baud
- Command interpreter:
  - `1-4`: Toggle individual LEDs
  - `A`: Turn all LEDs on
  - `O`: Turn all LEDs off
  - `S`: Display system status
  - `H`: Show help menu

## Hardware Requirements

- **STM32F407VG Microcontroller**
- **Cortex-M4 (168 MHz)**
- **ARM Cortex-M4 Compiler**: `arm-none-eabi-gcc` and `arm-none-eabi-g++`
- **STLink Programmer** (recommended for flashing)

## Building

### Prerequisites

Install ARM GCC toolchain:

```bash
# Ubuntu/Debian
sudo apt-get install gcc-arm-none-eabi g++-arm-none-eabi arm-none-eabi-gdb

# macOS (with Homebrew)
brew install arm-none-eabi-gcc

# Windows: Download from ARM developer website
```

### Compilation

```bash
# Navigate to project directory
cd STM32F407_Drivers

# Build the project
make

# View binary size information
make size

# Clean build artifacts
make clean
```

## Generated Files

After successful build:
- `build/bin/stm32f407_app.elf` - ELF executable (for debugging with GDB)
- `build/bin/stm32f407_app.hex` - Intel HEX format (for programming)
- `build/bin/stm32f407_app.bin` - Binary format (for programming)

## Programming the Device

### Using STLink Utility

```bash
# Flash binary file
st-flash write build/bin/stm32f407_app.bin 0x8000000

# Verify flash
st-info --probe
```

### Using OpenOCD

```bash
# Start OpenOCD server
openocd -f board/stm32f4discovery.cfg

# In another terminal, connect and flash
arm-none-eabi-gdb build/bin/stm32f407_app.elf
(gdb) target remote :3333
(gdb) load
(gdb) monitor reset
(gdb) quit
```

## UART Serial Communication

Connect to the device via serial terminal:

```bash
# Using minicom (Linux)
minicom -D /dev/ttyUSB0 -b 115200

# Using screen (Linux/macOS)
screen /dev/ttyUSB0 115200

# Using PuTTY (Windows)
# Configure: COM port, 115200 baud, 8 data bits, 1 stop bit, no parity
```

## Code Examples

### GPIO Basic Usage

```cpp
using namespace stm32f407;

// Create GPIO port instance
GPIO gpio(GPIOPort::GPIOD);

// Initialize pin as output
gpio.init(GPIOPin::PIN12, GPIOMode::OUTPUT, GPIOSpeed::MEDIUM);

// Control pin
gpio.writePin(GPIOPin::PIN12, true);   // Set high
gpio.togglePin(GPIOPin::PIN12);        // Toggle state
bool state = gpio.readPin(GPIOPin::PIN12);  // Read state
```

### UART Basic Usage

```cpp
using namespace stm32f407;

// Create UART instance
UART uart(UARTPort::UART2);

// Initialize at 115200 baud
uart.init(115200);

// Transmit
uart.sendByte('A');
uart.sendString("Hello World\r\n");

// Receive
if (uart.isDataAvailable()) {
    uint8_t byte = uart.receiveByte();
}
```

## STM32F407 Pin Mapping

### UART Pin Mappings
- **UART1**: TX=PA9 (AF7), RX=PA10 (AF7)
- **UART2**: TX=PA2 (AF7), RX=PA3 (AF7)
- **UART3**: TX=PB10 (AF7), RX=PB11 (AF7)
- **UART4**: TX=PA0 (AF8), RX=PA1 (AF8)
- **UART5**: TX=PC12 (AF8), RX=PD2 (AF8)
- **UART6**: TX=PC6 (AF8), RX=PC7 (AF8)

## Compiler Settings

- **C++ Standard**: C++17
- **Optimization**: O2 (balance between speed and size)
- **Architecture**: Cortex-M4 with FPv4-SP-D16 FPU
- **ABI**: Hard float
- **Exceptions**: Disabled (embedded environment)
- **RTTI**: Disabled (embedded environment)

## Register Access

All register access is done through volatile pointers to ensure the compiler doesn't optimize away critical hardware accesses. Memory-mapped I/O registers are properly configured for STM32F407.

## Customization

### Adding More GPIO Pins

Simply call `init()` with different pin numbers:

```cpp
gpio.init(GPIOPin::PIN0, GPIOMode::INPUT);
```

### Changing UART Baud Rate

Modify the `init()` call:

```cpp
uart.init(9600);  // 9600 baud
uart.init(230400); // 230400 baud
```

### Using Different Alternate Functions

Remap pins to different peripherals:

```cpp
gpio.setAlternateFunction(GPIOPin::PIN5, 2);  // Set AF2 for pin 5
```

## Limitations

- No interrupt-driven UART (polling only)
- No DMA support in this version
- Basic delay implementation (not precise)
- No external crystal configuration
- Assumes default system clock (168 MHz)

## Future Enhancements

- [ ] Interrupt support
- [ ] DMA transfers
- [ ] Timer/PWM support
- [ ] ADC driver
- [ ] I2C driver
- [ ] SPI driver
- [ ] Precise timing with system ticks

## License

This code is provided as-is for educational and development purposes.

## References

- [STM32F407 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
- [ARM Cortex-M4 Programmer's Manual](https://developer.arm.com/documentation/dui0553/latest/)
- [GCC ARM Embedded Documentation](https://gcc.gnu.org/onlinedocs/)

---

**Author**: Generated with C++17 Embedded Systems Best Practices  
**Date**: 2026
