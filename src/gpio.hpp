// gpio.hpp — thin, zero-cost C++17 wrapper over an STM32 GPIO port.
//
// Each method maps to a handful of register writes the compiler fully inlines.
// `constexpr` construction means a DigitalOut is just a (port, pin) pair with
// no hidden state and no vtable.
#pragma once

#include "registers.hpp"
#include <cstdint>

namespace stm32 {

enum class Mode : std::uint32_t {
    Input  = 0b00,
    Output = 0b01,
    Alt    = 0b10,
    Analog = 0b11,
};

// A single push-pull output pin on a given GPIO port.
class DigitalOut {
public:
    constexpr DigitalOut(Gpio& port, std::uint8_t pin) noexcept
        : port_{port}, pin_{pin} {}

    // Configure the pin as a push-pull output. Call once after the port clock
    // has been enabled.
    void init() const noexcept {
        const std::uint32_t two = pin_ * 2U;
        // MODER: clear the 2-bit field then set Output.
        port_.MODER = (port_.MODER & ~(0b11U << two))
                    | (static_cast<std::uint32_t>(Mode::Output) << two);
        port_.OTYPER  &= ~(1U << pin_);          // push-pull
        port_.OSPEEDR &= ~(0b11U << two);        // low speed is plenty for an LED
        port_.PUPDR   &= ~(0b11U << two);        // no pull
    }

    // BSRR is atomic: low half sets, high half (pin+16) resets. No read-modify-
    // write, so it is interrupt-safe without disabling IRQs.
    void set() const noexcept    { port_.BSRR = (1U << pin_); }
    void clear() const noexcept  { port_.BSRR = (1U << (pin_ + 16U)); }
    void toggle() const noexcept { port_.ODR ^= (1U << pin_); }

    void write(bool on) const noexcept { on ? set() : clear(); }
    bool read() const noexcept { return (port_.ODR >> pin_) & 1U; }

private:
    Gpio& port_;
    std::uint8_t pin_;
};

} // namespace stm32
