/**
 * @file gpio_mock.cpp
 * @brief GPIO Mock Implementation
 */

#include "gpio_mock.h"

namespace test {

// Initialize mock register structures
hal::GPIOx_TypeDef GPIOMock::mock_port_ = {};
hal::RCC_TypeDef GPIOMock::mock_rcc_ = {};

// Internal state tracking for each pin
static std::array<uint8_t, 16> pin_states = {};
static bool clock_enabled = false;

uint8_t GPIOMock::getPinState(uint8_t pin) {
    if (pin < 16) {
        return pin_states[pin];
    }
    return 0;
}

uint32_t GPIOMock::getMODERValue(uint8_t pin) {
    if (pin < 16) {
        return (mock_port_.MODER >> (pin * 2)) & 0x3U;
    }
    return 0;
}

uint32_t GPIOMock::getOSPEEDRValue(uint8_t pin) {
    if (pin < 16) {
        return (mock_port_.OSPEEDR >> (pin * 2)) & 0x3U;
    }
    return 0;
}

uint32_t GPIOMock::getPUPDRValue(uint8_t pin) {
    if (pin < 16) {
        return (mock_port_.PUPDR >> (pin * 2)) & 0x3U;
    }
    return 0;
}

void GPIOMock::resetMock() {
    mock_port_ = {};
    mock_rcc_ = {};
    pin_states.fill(0);
    clock_enabled = false;
}

bool GPIOMock::isClockEnabled() {
    return (mock_rcc_.AHB1ENR & hal::RCC_AHB1ENR_GPIODEN) != 0;
}

const std::array<uint8_t, 16>& GPIOMock::getInternalState() {
    return pin_states;
}

} // namespace test

// Override GPIO implementation for testing
namespace driver {

hal::GPIOx_TypeDef* GPIO::getPortForTesting() {
    return &test::GPIOMock::mock_port_;
}

hal::RCC_TypeDef* GPIO::getRCCForTesting() {
    return &test::GPIOMock::mock_rcc_;
}

} // namespace driver
