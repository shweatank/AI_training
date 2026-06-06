/**
 * @file gpio.cpp
 * @brief GPIO Driver Implementation
 */

#include "gpio.h"

namespace driver {

bool GPIO::initialized_ = false;

bool GPIO::init() {
    enablePortClock();
    initialized_ = true;
    return true;
}

bool GPIO::configurePin(uint8_t pin, 
                        GPIOMode mode, 
                        GPIOSpeed speed,
                        GPIOPull pull) {
    if (!isValidPin(pin)) {
        return false;
    }

    auto* port = getPortForTesting();
    if (!port) {
        return false;
    }

    // Clear and set MODER bits
    port->MODER &= ~(0x3U << (pin * 2));
    port->MODER |= (static_cast<uint32_t>(mode) << (pin * 2));

    // Set speed
    port->OSPEEDR &= ~(0x3U << (pin * 2));
    port->OSPEEDR |= (static_cast<uint32_t>(speed) << (pin * 2));

    // Set pull configuration
    port->PUPDR &= ~(0x3U << (pin * 2));
    port->PUPDR |= (static_cast<uint32_t>(pull) << (pin * 2));

    // Clear output type (push-pull)
    port->OTYPER &= ~(0x1U << pin);

    return true;
}

bool GPIO::setPin(uint8_t pin, GPIOLevel level) {
    if (!isValidPin(pin)) {
        return false;
    }

    auto* port = getPortForTesting();
    if (!port) {
        return false;
    }

    if (level == GPIOLevel::HIGH) {
        port->BSRRL = (1U << pin);
    } else {
        port->BSRRH = (1U << pin);
    }

    return true;
}

bool GPIO::togglePin(uint8_t pin) {
    if (!isValidPin(pin)) {
        return false;
    }

    auto* port = getPortForTesting();
    if (!port) {
        return false;
    }

    port->ODR ^= (1U << pin);
    return true;
}

uint8_t GPIO::readPin(uint8_t pin) {
    if (!isValidPin(pin)) {
        return 0;
    }

    auto* port = getPortForTesting();
    if (!port) {
        return 0;
    }

    return (port->IDR >> pin) & 0x1U;
}

bool GPIO::setPins(uint16_t mask) {
    auto* port = getPortForTesting();
    if (!port) {
        return false;
    }

    port->BSRRL = mask;
    return true;
}

bool GPIO::resetPins(uint16_t mask) {
    auto* port = getPortForTesting();
    if (!port) {
        return false;
    }

    port->BSRRH = mask;
    return true;
}

hal::GPIOx_TypeDef* GPIO::getPortForTesting() {
    return hal::getGPIOD();
}

hal::RCC_TypeDef* GPIO::getRCCForTesting() {
    return hal::getRCC();
}

void GPIO::enablePortClock() {
    auto* rcc = getRCCForTesting();
    if (!rcc) {
        return;
    }

    rcc->AHB1ENR |= hal::RCC_AHB1ENR_GPIODEN;
}

bool GPIO::isValidPin(uint8_t pin) {
    return pin < VALID_PINS;
}

} // namespace driver
