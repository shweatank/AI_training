/**
 * @file    i_uart.hpp
 * @brief   Abstract byte-oriented serial console (hardware abstraction layer).
 *
 * Transmission is treated as blocking-but-fast (polled TXE) and is only ever
 * invoked from the main loop / scheduler, never from interrupt context.
 * Reception is non-blocking: read() drains one byte from a buffer that the
 * platform fills in interrupt context, returning false when nothing is queued.
 *
 * The application performs all command parsing on bytes pulled through read();
 * the interface deliberately exposes no parsing or formatting of its own.
 */
#ifndef HAL_I_UART_HPP
#define HAL_I_UART_HPP

#include <cstdint>

namespace hal {

/// Abstract serial console: polled TX, buffered non-blocking RX.
class IUart {
public:
    virtual ~IUart() = default;

    /// Transmit a single byte (blocks only until the TX register is free).
    virtual void write(std::uint8_t byte) noexcept = 0;

    /// Transmit a NUL-terminated C string. Safe with nullptr.
    virtual void writeString(const char* str) noexcept = 0;

    /**
     * @brief Non-blocking single-byte read.
     * @param[out] out  Receives the next queued byte when one is available.
     * @return true if a byte was dequeued into @p out, false if none waiting.
     */
    virtual bool read(std::uint8_t& out) noexcept = 0;
};

} // namespace hal

#endif // HAL_I_UART_HPP
