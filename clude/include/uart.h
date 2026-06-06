/**
 * @file uart.h
 * @brief UART Driver for STM32F407
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef UART_H
#define UART_H

#include <cstdint>
#include <array>
#include "stm32f407xx.h"

namespace driver {

class UART {
public:
    static constexpr uint32_t BAUD_RATE = 115200;
    static constexpr uint32_t SYSTEM_CLOCK = 84000000; // 84 MHz APB2

    /**
     * @brief Initialize UART1
     * @return true if successful
     */
    static bool init();

    /**
     * @brief Send a single character
     * @param ch Character to send
     * @return true if successful
     */
    static bool sendChar(uint8_t ch);

    /**
     * @brief Send a string
     * @param str String to send
     * @return true if successful
     */
    static bool sendString(const char* str);

    /**
     * @brief Send data with length
     * @param data Pointer to data buffer
     * @param length Number of bytes to send
     * @return true if successful
     */
    static bool sendData(const uint8_t* data, uint16_t length);

    /**
     * @brief Receive a single character (blocking)
     * @param ch Reference to store received character
     * @param timeout_ms Timeout in milliseconds
     * @return true if successful
     */
    static bool receiveChar(uint8_t& ch, uint32_t timeout_ms = 1000);

    /**
     * @brief Check if data is available
     * @return true if data received
     */
    static bool isDataAvailable();

    /**
     * @brief Get UART peripheral for testing
     * @return Pointer to UART register structure
     */
    static hal::USART_TypeDef* getPeripheralForTesting();

    /**
     * @brief Get RCC for testing
     * @return Pointer to RCC register structure
     */
    static hal::RCC_TypeDef* getRCCForTesting();

private:
    static constexpr uint32_t TX_TIMEOUT = 1000;
    static constexpr uint32_t RX_TIMEOUT = 1000;
    static bool initialized_;

    /**
     * @brief Calculate baud rate register value
     */
    static uint32_t calculateBaudRate(uint32_t baud);

    /**
     * @brief Wait for TX complete
     */
    static bool waitTxComplete(uint32_t timeout);

    /**
     * @brief Wait for data available
     */
    static bool waitRxReady(uint32_t timeout);

    /**
     * @brief Enable UART clock
     */
    static void enableClock();
};

} // namespace driver

#endif // UART_H
