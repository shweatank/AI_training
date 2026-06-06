/**
 * @file    stm32f407_uart.cpp
 * @brief   Implementation of the STM32F407 USART driver.
 */
#include "stm32f407_uart.hpp"

namespace stm32f407 {

std::uint32_t Uart::computeBrr(std::uint32_t periph_clk,
                               std::uint32_t baud_rate) noexcept {
    if (baud_rate == 0) {
        return 0;
    }
    // Oversampling by 16 (OVER8 = 0): BRR = fCK / baud, rounded to nearest. The
    // mantissa occupies BRR[15:4] and the fraction BRR[3:0]; integer division of
    // (fCK + baud/2) by baud already packs both fields because the BRR LSB has
    // weight 1/16.
    return (periph_clk + (baud_rate / 2U)) / baud_rate;
}

void Uart::init(const UartConfig& cfg) noexcept {
    // Disable the USART before touching CR1 fields (RM0090 §30.6.4).
    regs_->CR1 = 0;

    regs_->BRR = computeBrr(cfg.periph_clk, cfg.baud_rate);

    // Stop bits live in CR2 STOP[13:12].
    std::uint32_t cr2 = regs_->CR2;
    cr2 &= ~usart_bits::kStopMask;
    cr2 |= (static_cast<std::uint32_t>(cfg.stop_bits) << usart_bits::kStopShift)
           & usart_bits::kStopMask;
    regs_->CR2 = cr2;

    // Build CR1: word length, parity, then enable TX/RX and the peripheral.
    std::uint32_t cr1 = 0;
    if (cfg.word_length == WordLength::Bits9) {
        cr1 |= usart_bits::kM;
    }
    switch (cfg.parity) {
        case Parity::Even: cr1 |= usart_bits::kPCE; break;
        case Parity::Odd:  cr1 |= usart_bits::kPCE | usart_bits::kPS; break;
        case Parity::None: default: break;
    }
    cr1 |= usart_bits::kTE | usart_bits::kRE; // transmitter + receiver
    cr1 |= usart_bits::kUE;                   // enable USART last
    regs_->CR1 = cr1;
}

void Uart::setRxInterrupt(bool enabled) noexcept {
    if (enabled) {
        regs_->CR1 |= usart_bits::kRXNEIE;
    } else {
        regs_->CR1 &= ~usart_bits::kRXNEIE;
    }
}

void Uart::writeByte(std::uint8_t byte) const noexcept {
    while ((regs_->SR & usart_bits::kTXE) == 0) {
        // spin until the transmit data register is empty
    }
    regs_->DR = static_cast<std::uint32_t>(byte) & 0x1FFU;
}

void Uart::writeBytes(const std::uint8_t* data, std::size_t len) const noexcept {
    for (std::size_t i = 0; i < len; ++i) {
        writeByte(data[i]);
    }
    while ((regs_->SR & usart_bits::kTC) == 0) {
    }
}

void Uart::writeString(const char* str) const noexcept {
    if (str == nullptr) return;
    while (*str != '\0') {
        writeByte(static_cast<std::uint8_t>(*str));
        ++str;
    }
    while ((regs_->SR & usart_bits::kTC) == 0) {
    }
}

bool Uart::rxReady() const noexcept {
    return (regs_->SR & usart_bits::kRXNE) != 0;
}

std::uint8_t Uart::readData() const noexcept {
    return static_cast<std::uint8_t>(regs_->DR & 0xFFU);
}

std::uint8_t Uart::readByte() const noexcept {
    while ((regs_->SR & usart_bits::kRXNE) == 0) {
        // spin until a byte arrives
    }
    return readData();
}

} // namespace stm32f407
