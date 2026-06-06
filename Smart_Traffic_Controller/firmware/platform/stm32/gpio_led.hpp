/**
 * @file    gpio_led.hpp
 * @brief   Adapts a GPIO output pin to the hal::ILed interface.
 *
 * Thin, header-only bridge: the application sees only hal::ILed, while this
 * class turns those calls into Gpio set/reset on a single pin. Active-high
 * wiring is assumed (the Discovery on-board LEDs source current from the pin).
 *
 * The caller is responsible for having configured the pin as a push-pull
 * output (and enabled the port clock) before constructing the adapter.
 */
#ifndef GPIO_LED_HPP
#define GPIO_LED_HPP

#include "hal/i_led.hpp"
#include "stm32f407_gpio.hpp"

#include <cstdint>

namespace stm32f407 {

/// hal::ILed backed by one active-high GPIO output pin.
class GpioLed final : public hal::ILed {
public:
    GpioLed(const Gpio& port, std::uint8_t pin) noexcept
        : port_(port), pin_(pin) {}

    void on() noexcept override  { port_.set(pin_); on_ = true; }
    void off() noexcept override { port_.reset(pin_); on_ = false; }

    void set(bool illuminated) noexcept override {
        illuminated ? on() : off();
    }

    bool isOn() const noexcept override { return on_; }

private:
    Gpio         port_;
    std::uint8_t pin_;
    bool         on_ = false;
};

} // namespace stm32f407

#endif // GPIO_LED_HPP
