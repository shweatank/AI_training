/**
 * @file    i_led.hpp
 * @brief   Abstract single-LED actuator (hardware abstraction layer).
 *
 * The application drives traffic-signal lamps exclusively through this
 * interface. The concrete implementation on the target (see
 * platform/stm32/gpio_led.hpp) wraps a GPIO pin; host unit tests substitute a
 * fake that simply records the last requested level. No GPIO register access
 * ever leaks into the application layer.
 *
 * C++17, freestanding: no exceptions, no RTTI, no dynamic allocation.
 */
#ifndef HAL_I_LED_HPP
#define HAL_I_LED_HPP

namespace hal {

/// Abstract on/off lamp output.
class ILed {
public:
    virtual ~ILed() = default;

    /// Drive the lamp on (illuminated).
    virtual void on() noexcept = 0;

    /// Drive the lamp off (dark).
    virtual void off() noexcept = 0;

    /// Drive the lamp to an explicit level (@p illuminated == true -> on).
    virtual void set(bool illuminated) noexcept = 0;

    /// True if the lamp is currently commanded on.
    virtual bool isOn() const noexcept = 0;
};

} // namespace hal

#endif // HAL_I_LED_HPP
