/**
 * @file main.cpp
 * @brief Main Application - LED Blinking with UART
 * @author Embedded Developer
 * @version 1.0
 */

#include "system_init.h"
#include "led_manager.h"
#include "uart.h"

// Forward declaration
extern "C" void _start();

int main() {
    // Initialize system clocks
    system::SystemInit::initClocks();
    system::SystemInit::enableAllClocks();

    // Initialize LED Manager (includes GPIO and UART)
    app::LEDManager::init();

    // Send startup message
    driver::UART::sendString("\r\n========================================\r\n");
    driver::UART::sendString("STM32F407 LED Blink Application v1.0\r\n");
    driver::UART::sendString("========================================\r\n");

    // Main application loop
    while (1) {
        // Blink all LEDs sequentially 3 times
        driver::UART::sendString("\nSequential blink: ");
        app::LEDManager::blinkAll(3, 150);
        driver::UART::sendString("Done\r\n");

        // Blink all LEDs simultaneously 3 times
        driver::UART::sendString("\nSimultaneous blink: ");
        app::LEDManager::blinkAllSimultaneous(3, 200);
        driver::UART::sendString("Done\r\n");

        // Individual LED control
        driver::UART::sendString("\nIndividual LED control:\r\n");

        app::LEDManager::turnOn(app::LEDColor::GREEN);
        driver::UART::sendString("Green ON\r\n");
        delayMs(500);

        app::LEDManager::turnOff(app::LEDColor::GREEN);
        app::LEDManager::turnOn(app::LEDColor::ORANGE);
        driver::UART::sendString("Orange ON\r\n");
        delayMs(500);

        app::LEDManager::turnOff(app::LEDColor::ORANGE);
        app::LEDManager::turnOn(app::LEDColor::RED);
        driver::UART::sendString("Red ON\r\n");
        delayMs(500);

        app::LEDManager::turnOff(app::LEDColor::RED);
        app::LEDManager::turnOn(app::LEDColor::BLUE);
        driver::UART::sendString("Blue ON\r\n");
        delayMs(500);

        app::LEDManager::turnOff(app::LEDColor::BLUE);

        // Wait before repeating
        driver::UART::sendString("\nWaiting 2 seconds...\r\n");
        delayMs(2000);
    }

    return 0;
}

/**
 * @brief Delay function in milliseconds
 * @param ms Milliseconds to delay
 */
void delayMs(uint32_t ms) {
    volatile uint32_t cycles = ms * 42000;  // Approximately 168 MHz / 4
    while (cycles--) {
        __asm__("nop");
    }
}

/**
 * @brief Reset handler (entry point)
 */
extern "C" void Reset_Handler() {
    main();

    // Halt if main returns
    while (1) {
        __asm__("nop");
    }
}

/**
 * @brief Default exception handler
 */
extern "C" void Default_Handler() {
    while (1) {
        __asm__("nop");
    }
}
