# STM32F407 Drivers Unit Tests

This directory contains comprehensive unit tests for the STM32F407 GPIO and UART drivers.

## Test Structure

- **gpio_test.cpp** - Tests for GPIO driver functionality
  - Enumeration value tests
  - GPIO port initialization
  - Pin configuration with various modes
  - Alternate function assignment
  - Pin read/write operations
  - Port read/write operations
  - Multi-pin and multi-port operations

- **uart_test.cpp** - Tests for UART driver functionality
  - Enumeration value tests
  - UART port initialization
  - Baud rate configuration
  - Data format configuration (data bits, stop bits, parity)
  - Byte transmission/reception
  - String transmission
  - Status checking (data available, transmitter busy)
  - Enable/disable operations
  - State transitions

- **CMakeLists.txt** - CMake build configuration for tests

## Prerequisites

### Linux (Ubuntu/Debian)
```bash
# Install Google Test
sudo apt-get install cmake libgtest-dev

# Build Google Test
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/libgtest*.a /usr/local/lib/
sudo mkdir -p /usr/local/include/gtest
sudo cp -r include/gtest/* /usr/local/include/gtest/
```

### macOS
```bash
# Install Google Test using Homebrew
brew install googletest
```

## Building and Running Tests

### Using the test script
```bash
cd tests
bash run_tests.sh
```

### Using CMake directly
```bash
cd tests
mkdir build_tests
cd build_tests
cmake ..
cmake --build .
ctest --output-on-failure
```

### Running individual tests
```bash
cd tests/build_tests
./gpio_test
./uart_test
```

## Test Coverage

### GPIO Tests (25 test cases)
- Port enumeration validation
- Pin enumeration validation
- Mode enumeration validation
- Output type enumeration validation
- Speed enumeration validation
- Pull configuration enumeration validation
- GPIO port initialization
- GPIO initialization with different modes
- Alternate function assignment
- Pin write operations
- Pin read operations
- Pin toggle operations
- Port write operations
- Port read operations
- Multiple GPIO port initialization
- Sequential pin operations
- All configuration combinations

### UART Tests (20 test cases)
- Port enumeration validation
- Data bits enumeration validation
- Stop bits enumeration validation
- Parity enumeration validation
- UART port initialization
- Initialization with different baud rates
- Initialization with all configuration options
- Single byte transmission
- Single byte reception
- String transmission
- Data availability check
- Transmitter busy check
- Enable/disable operations
- Multiple UART port initialization
- Common baud rate support
- All data configuration combinations
- Send/receive sequence
- Enable/disable with operations
- State transitions

## Test Results

Run tests with verbose output to see detailed results:
```bash
ctest --output-on-failure -V
```

## Notes

- These tests validate the interface and behavior of the drivers
- Tests are portable and can run on any platform with Google Test installed
- Tests do not require the actual STM32F407 hardware
- The tests use unit test patterns appropriate for embedded systems drivers

## Integration with CI/CD

You can integrate these tests into your CI/CD pipeline:

```yaml
# Example GitHub Actions workflow
- name: Run Unit Tests
  run: |
    cd tests
    bash run_tests.sh
```

## Troubleshooting

### CMake not found
Install CMake: `sudo apt-get install cmake` (Linux) or `brew install cmake` (macOS)

### Google Test not found
Ensure Google Test is properly installed and pkg-config can find it:
```bash
pkg-config --cflags --libs gtest
```

### Permission denied on run_tests.sh
Make the script executable:
```bash
chmod +x tests/run_tests.sh
```
