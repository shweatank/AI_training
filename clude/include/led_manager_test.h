/**
 * @file led_manager_test.h
 * @brief LED Manager Unit Tests
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef LED_MANAGER_TEST_H
#define LED_MANAGER_TEST_H

#include "unit_test.h"
#include "led_manager.h"
#include "gpio_mock.h"

namespace test {

/**
 * @brief LED Manager test suite
 */
class LEDManagerTest {
public:
    /**
     * @brief Run all LED Manager tests
     */
    static void runAllTests();

    /**
     * @brief Test LED Manager initialization
     */
    static void testInit();

    /**
     * @brief Test turning LED on
     */
    static void testTurnOn();

    /**
     * @brief Test turning LED off
     */
    static void testTurnOff();

    /**
     * @brief Test toggling LED
     */
    static void testToggle();

    /**
     * @brief Test blink once
     */
    static void testBlinkOnce();

    /**
     * @brief Test blink all sequentially
     */
    static void testBlinkAll();

    /**
     * @brief Test blink all simultaneously
     */
    static void testBlinkAllSimultaneous();

    /**
     * @brief Test getting LED pin
     */
    static void testGetLEDPin();

    /**
     * @brief Test getting LED count
     */
    static void testGetLEDCount();

    /**
     * @brief Test all LED colors
     */
    static void testAllLEDColors();

private:
    static const char* TEST_SUITE_NAME;

    /**
     * @brief Mock UART send for testing
     */
    static void mockUARTInit();
};

} // namespace test

#endif // LED_MANAGER_TEST_H
