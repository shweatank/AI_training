/**
 * @file    stm32f407_gpio.hpp
 * @brief   Type-safe C++17 GPIO driver for the STM32F407.
 *
 * Wraps a single GPIO port (a pointer to GpioRegs). It performs read-modify-
 * write on the 2-bit-per-pin configuration fields (MODER, PUPDR, OSPEEDR, AFR)
 * and uses the atomic BSRR register for set/reset so individual pin writes
 * never disturb their neighbours.
 *
 *  - Strongly-typed enums for every option (no magic numbers at call sites).
 *  - The driver never enables clocks itself; callers use Rcc so clock policy
 *    stays in one place. This keeps the driver pure register logic and easy to
 *    unit-test on the host.
 */
#ifndef STM32F407_GPIO_HPP
#define STM32F407_GPIO_HPP

#include "stm32f407_regs.hpp"
#include <cstdint>

namespace stm32f407 {

/// Pin direction / function (MODER field values).
enum class PinMode : std::uint32_t {
    Input     = 0x0,
    Output    = 0x1,
    Alternate = 0x2,
    Analog    = 0x3,
};

/// Output driver type (OTYPER bit values).
enum class OutputType : std::uint32_t {
    PushPull  = 0x0,
    OpenDrain = 0x1,
};

/// Output slew-rate (OSPEEDR field values).
enum class OutputSpeed : std::uint32_t {
    Low      = 0x0,
    Medium   = 0x1,
    High     = 0x2,
    VeryHigh = 0x3,
};

/// Internal pull resistor (PUPDR field values).
enum class Pull : std::uint32_t {
    None = 0x0,
    Up   = 0x1,
    Down = 0x2,
};

/// Alternate function selector (AFR field values, AF0..AF15).
enum class AltFunction : std::uint32_t {
    AF0  = 0,  AF1  = 1,  AF2  = 2,  AF3  = 3,
    AF4  = 4,  AF5  = 5,  AF6  = 6,  AF7  = 7,
    AF8  = 8,  AF9  = 9,  AF10 = 10, AF11 = 11,
    AF12 = 12, AF13 = 13, AF14 = 14, AF15 = 15,
};

/// Logic level of a pin.
enum class PinState : std::uint32_t {
    Low  = 0,
    High = 1,
};

/// Aggregated pin configuration passed to Gpio::configure().
struct PinConfig {
    PinMode     mode  = PinMode::Input;
    OutputType  otype = OutputType::PushPull;
    OutputSpeed speed = OutputSpeed::Low;
    Pull        pull  = Pull::None;
    AltFunction af    = AltFunction::AF0;
};

/**
 * @brief Driver for one GPIO port.
 *
 * Construct from a register-block pointer. On target use e.g. Gpio(GPIOD());
 * in tests pass a pointer to a mock GpioRegs living in host RAM.
 */
class Gpio {
public:
    /// Number of pins per port on the STM32F407.
    static constexpr std::uint8_t kPinCount = 16;

    explicit Gpio(GpioRegs* port) noexcept : port_(port) {}

    /// Apply a full configuration to @p pin (0..15). Out-of-range pins ignored.
    void configure(std::uint8_t pin, const PinConfig& cfg) const noexcept;

    /// Set @p pin high using the atomic BSRR register.
    void set(std::uint8_t pin) const noexcept;

    /// Set @p pin low using the atomic BSRR register.
    void reset(std::uint8_t pin) const noexcept;

    /// Drive @p pin to an explicit @p state.
    void write(std::uint8_t pin, PinState state) const noexcept;

    /// Toggle @p pin's current output level.
    void toggle(std::uint8_t pin) const noexcept;

    /// Read the live input level of @p pin.
    PinState read(std::uint8_t pin) const noexcept;

private:
    GpioRegs* port_;

    /// Write a 2-bit field for @p pin within @p reg.
    static void write2bit(reg32_t& reg, std::uint8_t pin, std::uint32_t value) noexcept;
};

} // namespace stm32f407

#endif // STM32F407_GPIO_HPP
