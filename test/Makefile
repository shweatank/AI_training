# Makefile — build & run the host-side unit tests with the native g++.
#
#   make        compile and run all tests
#   make clean  remove the test binaries

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wshadow -Wconversion -I../src -g
BINS     = test_gpio test_blink
HDRS     = ../src/gpio.hpp ../src/registers.hpp ../src/blink.hpp

.PHONY: test clean
test: $(BINS)
	@echo "=== GPIO driver tests ==="   && ./test_gpio
	@echo "=== Blink pattern tests ===" && ./test_blink

test_gpio: test_gpio.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) test_gpio.cpp -o $@

test_blink: test_blink.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) test_blink.cpp -o $@

clean:
	rm -f $(BINS)
