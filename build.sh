#!/bin/bash

# Quick setup and build script for STM32F407 project
# This script checks dependencies and builds the project

set -e

echo "=========================================="
echo "STM32F407 Drivers - Quick Setup Script"
echo "=========================================="
echo ""

# Check for ARM toolchain
echo "[1/3] Checking for ARM toolchain..."
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "ERROR: arm-none-eabi-gcc not found!"
    echo "Install it with:"
    echo "  Ubuntu/Debian: sudo apt-get install gcc-arm-none-eabi g++-arm-none-eabi"
    echo "  macOS: brew install arm-none-eabi-gcc"
    echo "  Windows: Download from ARM developer website"
    exit 1
fi

ARM_VERSION=$(arm-none-eabi-gcc --version | head -n1)
echo "✓ Found: $ARM_VERSION"
echo ""

# Check for make
echo "[2/3] Checking for make..."
if ! command -v make &> /dev/null; then
    echo "ERROR: make not found!"
    echo "Install it with:"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  macOS: brew install make"
    exit 1
fi

MAKE_VERSION=$(make --version | head -n1)
echo "✓ Found: $MAKE_VERSION"
echo ""

# Build project
echo "[3/3] Building project..."
make clean
make

echo ""
echo "=========================================="
echo "Build completed successfully!"
echo "=========================================="
echo ""
echo "Generated files:"
echo "  • build/bin/stm32f407_app.elf  (ELF executable)"
echo "  • build/bin/stm32f407_app.hex  (Intel HEX format)"
echo "  • build/bin/stm32f407_app.bin  (Binary format)"
echo ""
echo "To flash to device:"
echo "  st-flash write build/bin/stm32f407_app.bin 0x8000000"
echo ""
echo "To view help:"
echo "  make help"
echo ""
