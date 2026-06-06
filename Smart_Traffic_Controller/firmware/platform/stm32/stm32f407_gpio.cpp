/**
 * @file    stm32f407_gpio.cpp
 * @brief   Implementation of the STM32F407 GPIO driver.
 */
#include "stm32f407_gpio.hpp"

namespace stm32f407 {

void Gpio::write2bit(reg32_t& reg, std::uint8_t pin, std::uint32_t value) noexcept {
    const std::uint32_t shift = static_cast<std::uint32_t>(pin) * 2U;
    std::uint32_t tmp = reg;
    tmp &= ~(0x3U << shift);        // clear the field
    tmp |= (value & 0x3U) << shift; // write the new value
    reg = tmp;
}

void Gpio::configure(std::uint8_t pin, const PinConfig& cfg) const noexcept {
    if (pin >= kPinCount) {
        return; // out-of-range pin: ignore rather than corrupt neighbouring fields
    }

    write2bit(port_->MODER, pin, static_cast<std::uint32_t>(cfg.mode));
    write2bit(port_->OSPEEDR, pin, static_cast<std::uint32_t>(cfg.speed));
    write2bit(port_->PUPDR, pin, static_cast<std::uint32_t>(cfg.pull));

    // OTYPER is 1 bit per pin.
    if (cfg.otype == OutputType::OpenDrain) {
        port_->OTYPER |= (1U << pin);
    } else {
        port_->OTYPER &= ~(1U << pin);
    }

    // Alternate function: 4 bits per pin, split across AFR[0] (pins 0-7) and
    // AFR[1] (pins 8-15).
    const std::uint32_t idx   = pin >> 3;          // 0 or 1
    const std::uint32_t shift = (pin & 0x7U) * 4U; // bit offset within word
    std::uint32_t afr = port_->AFR[idx];
    afr &= ~(0xFU << shift);
    afr |= (static_cast<std::uint32_t>(cfg.af) & 0xFU) << shift;
    port_->AFR[idx] = afr;
}

void Gpio::set(std::uint8_t pin) const noexcept {
    if (pin >= kPinCount) return;
    port_->BSRR = (1U << pin); // lower 16 bits: set
}

void Gpio::reset(std::uint8_t pin) const noexcept {
    if (pin >= kPinCount) return;
    port_->BSRR = (1U << (pin + 16U)); // upper 16 bits: reset
}

void Gpio::write(std::uint8_t pin, PinState state) const noexcept {
    (state == PinState::High) ? set(pin) : reset(pin);
}

void Gpio::toggle(std::uint8_t pin) const noexcept {
    if (pin >= kPinCount) return;
    if (port_->ODR & (1U << pin)) {
        reset(pin);
    } else {
        set(pin);
    }
}

PinState Gpio::read(std::uint8_t pin) const noexcept {
    if (pin >= kPinCount) return PinState::Low;
    return (port_->IDR & (1U << pin)) ? PinState::High : PinState::Low;
}

} // namespace stm32f407
