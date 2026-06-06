/**
 * @file gpio_test.h
 * @brief GPIO Driver Unit Tests
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef GPIO_TEST_H
#define GPIO_TEST_H

#include "unit_test.h"
#include "gpio.h"
#include "gpio_mock.h"

namespace test {

/**
 * @brief GPIO test suite
 */
class GPIOTest {
public:
    /**
     * @brief Run all GPIO tests
     */
    static void runAllTests();

    /**
     * @brief Test GPIO initialization
     */
    static void testInit();

    /**
     * @brief Test GPIO pin configuration
     */
    static void testConfigurePin();

    /**
     * @brief Test setting GPIO pin to HIGH
     */
    static void testSetPinHigh();

    /**
     * @brief Test setting GPIO pin to LOW
     */
    static void testSetPinLow();

    /**
     * @brief Test toggling GPIO pin
     */
    static void testTogglePin();

    /**
     * @brief Test reading GPIO pin
     */
    static void testReadPin();

    /**
     * @brief Test setting multiple pins
     */
    static void testSetPins();

    /**
     * @brief Test resetting multiple pins
     */
    static void testResetPins();

    /**
     * @brief Test invalid pin handling
     */
    static void testInvalidPin();

    /**
     * @brief Test GPIO mode configuration
     */
    static void testModeConfiguration();

    /**
     * @brief Test GPIO speed configuration
     */
    static void testSpeedConfiguration();

    /**
     * @brief Test GPIO pull configuration
     */
    static void testPullConfiguration();

private:
    static const char* TEST_SUITE_NAME;
};

} // namespace test

#endif // GPIO_TEST_H
