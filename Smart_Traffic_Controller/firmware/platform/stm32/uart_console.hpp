/**
 * @file    uart_console.hpp
 * @brief   Interrupt-RX serial console adapting Uart to hal::IUart.
 *
 * Transmission is polled (blocking only until TXE) and only ever called from
 * the main loop. Reception is interrupt-driven: the USART RXNE interrupt pushes
 * each received byte into an internal ring buffer, and read() drains one byte
 * from that buffer without blocking. No parsing or formatting happens in the
 * ISR — it does the bare minimum (read DR, push) per the interrupt rules.
 *
 * A single USART2 instance is supported: attachUsart2Isr() registers this
 * console as the target of the USART2_IRQHandler defined in uart_console.cpp.
 */
#ifndef UART_CONSOLE_HPP
#define UART_CONSOLE_HPP

#include "hal/i_uart.hpp"
#include "ring_buffer.hpp"
#include "stm32f407_uart.hpp"

#include <cstdint>

namespace stm32f407 {

/// hal::IUart backed by a USART with interrupt-driven RX buffering.
class UartConsole final : public hal::IUart {
public:
    /// RX buffer capacity (bytes). One slot is reserved by the ring buffer.
    static constexpr std::size_t kRxCapacity = 128;

    explicit UartConsole(const Uart& uart) noexcept : uart_(uart) {}

    void write(std::uint8_t byte) noexcept override { uart_.writeByte(byte); }
    void writeString(const char* str) noexcept override { uart_.writeString(str); }

    bool read(std::uint8_t& out) noexcept override { return rx_.pop(out); }

    /**
     * @brief Called from the USART RX interrupt: read DR and enqueue the byte.
     *
     * Kept minimal and non-blocking. A full buffer drops the byte rather than
     * stalling the ISR.
     */
    void onRxInterrupt() noexcept {
        if (uart_.rxReady()) {
            const std::uint8_t byte = uart_.readData();
            (void)rx_.push(byte); // drop on overflow; never block in an ISR
        }
    }

    /// Number of bytes currently buffered (mainly for diagnostics/tests).
    std::size_t rxPending() const noexcept { return rx_.size(); }

private:
    Uart                              uart_;
    app::RingBuffer<std::uint8_t, kRxCapacity + 1> rx_;
};

/// Register @p console as the USART2 RX-interrupt target and enable the IRQ in
/// the NVIC. Pass nullptr to detach. Call after Uart::setRxInterrupt(true).
void attachUsart2Isr(UartConsole* console) noexcept;

} // namespace stm32f407

#endif // UART_CONSOLE_HPP
