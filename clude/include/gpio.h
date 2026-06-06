/**
 * @file gpio.h
 * @brief GPIO Driver for STM32F407
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef GPIO_H
#define GPIO_H

#include <cstdint>
#include "stm32f407xx.h"

namespace driver {

enum class GPIOMode : uint8_t {
    INPUT = 0,
    OUTPUT = 1,
    ALTERNATE = 2,
    ANALOG = 3
};

enum class GPIOSpeed : uint8_t {
    SLOW = 0,
    MEDIUM = 1,
    FAST = 2,
    VERY_FAST = 3
};

enum class GPIOPull : uint8_t {
    NO_PULL = 0,
    PULL_UP = 1,
    PULL_DOWN = 2
};

enum class GPIOLevel : uint8_t {
    LOW = 0,
    HIGH = 1
};

class GPIO {
public:
    /**
     * @brief Initialize GPIO port
     * @return true if successful
     */
    static bool init();

    /**
     * @brief Configure a GPIO pin
     * @param pin Pin number (0-15)
     * @param mode Pin mode (input/output/alternate/analog)
     * @param speed Output speed
     * @param pull Pull configuration
     * @return true if successful
     */
    static bool configurePin(uint8_t pin, 
                            GPIOMode mode, 
                            GPIOSpeed speed = GPIOSpeed::MEDIUM,
                            GPIOPull pull = GPIOPull::NO_PULL);

    /**
     * @brief Set GPIO pin level
     * @param pin Pin number
     * @param level HIGH or LOW
     * @return true if successful
     */
    static bool setPin(uint8_t pin, GPIOLevel level);

    /**
     * @brief Toggle GPIO pin
     * @param pin Pin number
     * @return true if successful
     */
    static bool togglePin(uint8_t pin);

    /**
     * @brief Read GPIO pin level
     * @param pin Pin number
     * @return Pin level (0 or 1)
     */
    static uint8_t readPin(uint8_t pin);

    /**
     * @brief Set multiple pins at once (atomic operation)
     * @param mask Pins to set (bit mask)
     * @return true if successful
     */
    static bool setPins(uint16_t mask);

    /**
     * @brief Reset multiple pins at once (atomic operation)
     * @param mask Pins to reset (bit mask)
     * @return true if successful
     */
    static bool resetPins(uint16_t mask);

    /**
     * @brief Get GPIO port for testing
     * @return Pointer to GPIO register structure
     */
    static hal::GPIOx_TypeDef* getPortForTesting();

    /**
     * @brief Get RCC for testing
     * @return Pointer to RCC register structure
     */
    static hal::RCC_TypeDef* getRCCForTesting();

private:
    static constexpr uint8_t VALID_PINS = 16;
    static bool initialized_;

    /**
     * @brief Enable GPIO port clock
     */
    static void enablePortClock();

    /**
     * @brief Validate pin number
     */
    static bool isValidPin(uint8_t pin);
};

} // namespace driver

#endif // GPIO_H
