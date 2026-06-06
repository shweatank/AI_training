/**
 * @file gpio_mock.h
 * @brief GPIO Mock for Unit Testing
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef GPIO_MOCK_H
#define GPIO_MOCK_H

#include <cstdint>
#include <array>
#include "gpio.h"
#include "stm32f407xx.h"

namespace test {

/**
 * @brief Mock GPIO implementation for testing
 */
class GPIOMock : public driver::GPIO {
public:
    // Mock register structures for testing
    static hal::GPIOx_TypeDef mock_port_;
    static hal::RCC_TypeDef mock_rcc_;

    /**
     * @brief Get state of a GPIO pin
     * @param pin Pin number
     * @return Pin state (0 or 1)
     */
    static uint8_t getPinState(uint8_t pin);

    /**
     * @brief Get MODER register value
     * @param pin Pin number
     * @return MODER value for the pin
     */
    static uint32_t getMODERValue(uint8_t pin);

    /**
     * @brief Get OSPEEDR register value
     * @param pin Pin number
     * @return OSPEEDR value for the pin
     */
    static uint32_t getOSPEEDRValue(uint8_t pin);

    /**
     * @brief Get PUPDR register value
     * @param pin Pin number
     * @return PUPDR value for the pin
     */
    static uint32_t getPUPDRValue(uint8_t pin);

    /**
     * @brief Reset mock state
     */
    static void resetMock();

    /**
     * @brief Check if port clock is enabled
     * @return true if clock is enabled
     */
    static bool isClockEnabled();

    /**
     * @brief Get internal state for verification
     */
    static const std::array<uint8_t, 16>& getInternalState();
};

} // namespace test

#endif // GPIO_MOCK_H
