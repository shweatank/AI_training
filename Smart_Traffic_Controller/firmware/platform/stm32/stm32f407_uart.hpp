/**
 * @file    stm32f407_uart.hpp
 * @brief   C++17 USART driver for the STM32F407: polled TX, interrupt-capable RX.
 *
 * Computes BRR from the peripheral clock and baud rate (oversampling by 16),
 * configures word length / parity / stop bits, and provides blocking byte and
 * string transmit. Receive can be enabled in interrupt mode (RXNEIE); the ISR
 * glue lives in uart_console.cpp, which keeps this driver pure register logic.
 *
 * The driver is independent of pin muxing: callers configure the TX/RX pins to
 * the correct alternate function via the Gpio driver beforehand.
 */
#ifndef STM32F407_UART_HPP
#define STM32F407_UART_HPP

#include "stm32f407_regs.hpp"
#include <cstddef>
#include <cstdint>

namespace stm32f407 {

enum class WordLength : std::uint32_t {
    Bits8 = 0,
    Bits9 = 1,
};

enum class Parity : std::uint32_t {
    None = 0,
    Even = 1,
    Odd  = 2,
};

enum class StopBits : std::uint32_t {
    One = 0x0,
    Two = 0x2, // STOP[1:0] = 0b10
};

/// UART configuration. Defaults to the classic 115200-8N1 line setting.
struct UartConfig {
    std::uint32_t baud_rate   = 115200;
    std::uint32_t periph_clk  = 16000000; ///< APBx clock feeding the USART (Hz)
    WordLength    word_length = WordLength::Bits8;
    Parity        parity      = Parity::None;
    StopBits      stop_bits   = StopBits::One;
};

/// USART driver bound to one USART instance.
class Uart {
public:
    explicit Uart(UsartRegs* regs) noexcept : regs_(regs) {}

    /// Configure line parameters and enable TX+RX. Disables the USART while
    /// reprogramming control registers, then re-enables it.
    void init(const UartConfig& cfg) noexcept;

    /// Enable (or disable) the RXNE receive interrupt.
    void setRxInterrupt(bool enabled) noexcept;

    /// Blocking transmit of a single byte (waits for TXE).
    void writeByte(std::uint8_t byte) const noexcept;

    /// Blocking transmit of @p len bytes from @p data.
    void writeBytes(const std::uint8_t* data, std::size_t len) const noexcept;

    /// Blocking transmit of a NUL-terminated C string. Safe with nullptr.
    void writeString(const char* str) const noexcept;

    /// Blocking receive of a single byte (waits for RXNE).
    std::uint8_t readByte() const noexcept;

    /// True if a received byte is waiting in DR (RXNE set).
    bool rxReady() const noexcept;

    /// Read DR unconditionally (clears RXNE). Intended for the RX ISR, which
    /// has already confirmed RXNE; never blocks.
    std::uint8_t readData() const noexcept;

    /// Compute the BRR value for a given clock/baud (oversampling by 16).
    /// Static so it can be unit-tested without hardware.
    static std::uint32_t computeBrr(std::uint32_t periph_clk,
                                    std::uint32_t baud_rate) noexcept;

private:
    UsartRegs* regs_;
};

} // namespace stm32f407

#endif // STM32F407_UART_HPP
