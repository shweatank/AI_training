/**
 * @file led_manager.h
 * @brief LED Manager for STM32F407 Discovery Board
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <cstdint>
#include <array>
#include "gpio.h"
#include "uart.h"

namespace app {

enum class LEDColor : uint8_t {
    GREEN = 0,
    ORANGE = 1,
    RED = 2,
    BLUE = 3,
    ALL = 4
};

class LEDManager {
public:
    /**
     * @brief Initialize LED Manager
     * @return true if successful
     */
    static bool init();

    /**
     * @brief Turn on an LED
     * @param color LED color to turn on
     * @return true if successful
     */
    static bool turnOn(LEDColor color);

    /**
     * @brief Turn off an LED
     * @param color LED color to turn off
     * @return true if successful
     */
    static bool turnOff(LEDColor color);

    /**
     * @brief Toggle an LED
     * @param color LED color to toggle
     * @return true if successful
     */
    static bool toggle(LEDColor color);

    /**
     * @brief Blink an LED once
     * @param color LED color to blink
     * @param duration_ms Duration of blink in milliseconds
     * @return true if successful
     */
    static bool blinkOnce(LEDColor color, uint32_t duration_ms = 100);

    /**
     * @brief Blink all LEDs sequentially
     * @param count Number of times to blink
     * @param duration_ms Duration of each blink
     * @return true if successful
     */
    static bool blinkAll(uint8_t count = 3, uint32_t duration_ms = 100);

    /**
     * @brief Blink all LEDs simultaneously
     * @param count Number of times to blink
     * @param duration_ms Duration of each blink
     * @return true if successful
     */
    static bool blinkAllSimultaneous(uint8_t count = 3, uint32_t duration_ms = 100);

    /**
     * @brief Get LED pin for a specific color
     * @param color LED color
     * @return Pin number
     */
    static uint8_t getLEDPin(LEDColor color);

    /**
     * @brief Get number of LEDs
     * @return Number of available LEDs
     */
    static uint8_t getLEDCount();

    /**
     * @brief Send status via UART
     * @param color LED color
     * @param state 1 for ON, 0 for OFF
     * @return true if successful
     */
    static bool sendStatus(LEDColor color, uint8_t state);

private:
    static constexpr uint8_t LED_COUNT = 4;
    static constexpr std::array<uint8_t, LED_COUNT> LED_PINS = {
        12, // Green - PD12
        13, // Orange - PD13
        14, // Red - PD14
        15  // Blue - PD15
    };

    static bool initialized_;

    /**
     * @brief Convert LEDColor to array index
     */
    static uint8_t colorToIndex(LEDColor color);

    /**
     * @brief Delay function (platform dependent)
     */
    static void delay_ms(uint32_t ms);

    /**
     * @brief Log message via UART
     */
    static void logMessage(const char* msg);
};

} // namespace app

#endif // LED_MANAGER_H
