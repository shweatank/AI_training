/**
 * @file led_manager.cpp
 * @brief LED Manager Implementation
 */

#include "led_manager.h"

namespace app {

bool LEDManager::initialized_ = false;

bool LEDManager::init() {
    // Initialize GPIO
    if (!driver::GPIO::init()) {
        logMessage("GPIO init failed\r\n");
        return false;
    }

    // Initialize UART
    if (!driver::UART::init()) {
        logMessage("UART init failed\r\n");
        return false;
    }

    // Configure all LED pins as output
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        if (!driver::GPIO::configurePin(LED_PINS[i], 
                                        driver::GPIOMode::OUTPUT,
                                        driver::GPIOSpeed::FAST,
                                        driver::GPIOPull::NO_PULL)) {
            logMessage("GPIO pin config failed\r\n");
            return false;
        }
        // Turn off all LEDs initially
        driver::GPIO::setPin(LED_PINS[i], driver::GPIOLevel::LOW);
    }

    initialized_ = true;
    driver::UART::sendString("LED Manager initialized\r\n");
    return true;
}

bool LEDManager::turnOn(LEDColor color) {
    if (!initialized_) {
        return false;
    }

    uint8_t pin = getLEDPin(color);
    if (pin == 0xFF) {
        return false;
    }

    bool result = driver::GPIO::setPin(pin, driver::GPIOLevel::HIGH);
    if (result) {
        sendStatus(color, 1);
    }
    return result;
}

bool LEDManager::turnOff(LEDColor color) {
    if (!initialized_) {
        return false;
    }

    uint8_t pin = getLEDPin(color);
    if (pin == 0xFF) {
        return false;
    }

    bool result = driver::GPIO::setPin(pin, driver::GPIOLevel::LOW);
    if (result) {
        sendStatus(color, 0);
    }
    return result;
}

bool LEDManager::toggle(LEDColor color) {
    if (!initialized_) {
        return false;
    }

    uint8_t pin = getLEDPin(color);
    if (pin == 0xFF) {
        return false;
    }

    return driver::GPIO::togglePin(pin);
}

bool LEDManager::blinkOnce(LEDColor color, uint32_t duration_ms) {
    if (!initialized_) {
        return false;
    }

    if (!turnOn(color)) {
        return false;
    }

    delay_ms(duration_ms);

    if (!turnOff(color)) {
        return false;
    }

    return true;
}

bool LEDManager::blinkAll(uint8_t count, uint32_t duration_ms) {
    if (!initialized_) {
        return false;
    }

    for (uint8_t i = 0; i < count; i++) {
        for (uint8_t j = 0; j < LED_COUNT; j++) {
            LEDColor colors[] = {LEDColor::GREEN, LEDColor::ORANGE, 
                                LEDColor::RED, LEDColor::BLUE};
            if (!blinkOnce(colors[j], duration_ms)) {
                return false;
            }
            delay_ms(duration_ms);
        }
    }

    return true;
}

bool LEDManager::blinkAllSimultaneous(uint8_t count, uint32_t duration_ms) {
    if (!initialized_) {
        return false;
    }

    for (uint8_t i = 0; i < count; i++) {
        // Turn all LEDs on
        uint16_t mask = 0;
        for (uint8_t j = 0; j < LED_COUNT; j++) {
            mask |= (1U << LED_PINS[j]);
        }
        driver::GPIO::setPins(mask);

        delay_ms(duration_ms);

        // Turn all LEDs off
        driver::GPIO::resetPins(mask);

        delay_ms(duration_ms);
    }

    return true;
}

uint8_t LEDManager::getLEDPin(LEDColor color) {
    uint8_t index = colorToIndex(color);
    if (index >= LED_COUNT) {
        return 0xFF;
    }
    return LED_PINS[index];
}

uint8_t LEDManager::getLEDCount() {
    return LED_COUNT;
}

bool LEDManager::sendStatus(LEDColor color, uint8_t state) {
    if (!initialized_) {
        return false;
    }

    const char* colors[] = {"GREEN", "ORANGE", "RED", "BLUE"};
    uint8_t index = colorToIndex(color);

    if (index >= LED_COUNT) {
        return false;
    }

    // Format: "LED: [COLOR] - [ON/OFF]\r\n"
    driver::UART::sendString("LED: ");
    driver::UART::sendString(colors[index]);
    driver::UART::sendString(" - ");
    driver::UART::sendString(state ? "ON\r\n" : "OFF\r\n");

    return true;
}

uint8_t LEDManager::colorToIndex(LEDColor color) {
    if (color == LEDColor::ALL) {
        return LED_COUNT;  // Invalid for individual operations
    }
    return static_cast<uint8_t>(color);
}

void LEDManager::delay_ms(uint32_t ms) {
    // Simple busy-wait delay (in real application, use systick)
    // Assuming ~1 cycle per iteration at ~168 MHz
    volatile uint32_t cycles = ms * 42000;  // Rough estimate
    while (cycles--) {
        __asm__("nop");
    }
}

void LEDManager::logMessage(const char* msg) {
    if (initialized_) {
        driver::UART::sendString(msg);
    }
}

} // namespace app
