/**
 * @file uart.cpp
 * @brief UART Driver Implementation
 */

#include "uart.h"

namespace driver {

bool UART::initialized_ = false;

bool UART::init() {
    enableClock();

    auto* uart = getPeripheralForTesting();
    if (!uart) {
        return false;
    }

    // Calculate and set baud rate
    uint32_t brr_value = calculateBaudRate(BAUD_RATE);
    uart->BRR = brr_value;

    // Configure UART: 8 data bits, 1 stop bit, no parity
    uart->CR1 = 0;
    uart->CR1 |= (hal::USART_CR1_TE | hal::USART_CR1_RE);
    uart->CR2 = 0;
    uart->CR3 = 0;

    // Enable UART
    uart->CR1 |= hal::USART_CR1_UE;

    initialized_ = true;
    return true;
}

bool UART::sendChar(uint8_t ch) {
    auto* uart = getPeripheralForTesting();
    if (!uart) {
        return false;
    }

    if (!waitTxComplete(TX_TIMEOUT)) {
        return false;
    }

    uart->DR = ch;

    return true;
}

bool UART::sendString(const char* str) {
    if (!str) {
        return false;
    }

    while (*str != '\0') {
        if (!sendChar(static_cast<uint8_t>(*str))) {
            return false;
        }
        str++;
    }

    return true;
}

bool UART::sendData(const uint8_t* data, uint16_t length) {
    if (!data || length == 0) {
        return false;
    }

    for (uint16_t i = 0; i < length; i++) {
        if (!sendChar(data[i])) {
            return false;
        }
    }

    return true;
}

bool UART::receiveChar(uint8_t& ch, uint32_t timeout_ms) {
    if (!waitRxReady(timeout_ms)) {
        return false;
    }

    auto* uart = getPeripheralForTesting();
    if (!uart) {
        return false;
    }

    ch = uart->DR & 0xFF;
    return true;
}

bool UART::isDataAvailable() {
    auto* uart = getPeripheralForTesting();
    if (!uart) {
        return false;
    }

    return (uart->SR & hal::USART_SR_RXNE) != 0;
}

hal::USART_TypeDef* UART::getPeripheralForTesting() {
    return hal::getUSART1();
}

hal::RCC_TypeDef* UART::getRCCForTesting() {
    return hal::getRCC();
}

uint32_t UART::calculateBaudRate(uint32_t baud) {
    // BRR = PCLK / (16 * USART_Baud)
    // For APB2: PCLK2 = 84 MHz
    return SYSTEM_CLOCK / (16 * baud);
}

bool UART::waitTxComplete(uint32_t timeout) {
    auto* uart = getPeripheralForTesting();
    if (!uart) {
        return false;
    }

    uint32_t counter = 0;
    while ((uart->SR & hal::USART_SR_TXE) == 0) {
        if (counter >= timeout) {
            return false;
        }
        counter++;
    }

    return true;
}

bool UART::waitRxReady(uint32_t timeout) {
    auto* uart = getPeripheralForTesting();
    if (!uart) {
        return false;
    }

    uint32_t counter = 0;
    while ((uart->SR & hal::USART_SR_RXNE) == 0) {
        if (counter >= timeout) {
            return false;
        }
        counter++;
    }

    return true;
}

void UART::enableClock() {
    auto* rcc = getRCCForTesting();
    if (!rcc) {
        return;
    }

    rcc->APB2ENR |= hal::RCC_APB2ENR_USART1EN;
}

} // namespace driver
