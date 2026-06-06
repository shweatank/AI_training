#!/bin/bash

# STM32F407 Drivers Unit Test Build Script
# This script builds and runs unit tests for the driver library

set -e

TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$TEST_DIR/build_tests"

echo "=========================================="
echo "STM32F407 Drivers Unit Tests"
echo "=========================================="

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake is not installed. Please install CMake to run tests."
    exit 1
fi

# Check if Google Test is installed
if ! pkg-config --exists gtest; then
    echo "WARNING: Google Test (gtest) is not installed."
    echo "Install it using: sudo apt-get install libgtest-dev"
    echo "Or: brew install googletest (on macOS)"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
echo "Running CMake configuration..."
cmake ..

# Build tests
echo "Building tests..."
cmake --build .

# Run tests
echo ""
echo "=========================================="
echo "Running Unit Tests..."
echo "=========================================="
ctest --output-on-failure -V

echo ""
echo "=========================================="
echo "Test run complete!"
echo "=========================================="
