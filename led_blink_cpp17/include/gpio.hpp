#ifndef GPIO_HPP
#define GPIO_HPP

// ============================================================================
//  STM32F407 GPIO driver - embedded C++17
//
//  Design note (testability):
//    The driver operates on a GpioRegisters* that is injected at construction.
//    - On hardware:  Gpio::at(Port) enables the port clock and points the
//                    driver at the real peripheral block.
//    - In unit tests: construct Gpio with the address of a plain
//                    GpioRegisters value living in host RAM, so register
//                    effects can be inspected without any hardware access.
// ============================================================================

#include <cstdint>

#include "stm32f407_registers.hpp"

namespace stm32f407 {

// GPIO ports A..I.
enum class Port : std::uint8_t {
    A = 0, B, C, D, E, F, G, H, I
};

// GPIO pins 0..15.
enum class Pin : std::uint8_t {
    P0 = 0, P1, P2, P3, P4, P5, P6, P7,
    P8, P9, P10, P11, P12, P13, P14, P15
};

// Pin direction / function (MODER field values).
enum class Mode : std::uint8_t {
    Input     = 0b00,
    Output    = 0b01,
    Alternate = 0b10,
    Analog    = 0b11
};

// Output driver type (OTYPER field values).
enum class OutputType : std::uint8_t {
    PushPull  = 0,
    OpenDrain = 1
};

// Output slew-rate (OSPEEDR field values).
enum class Speed : std::uint8_t {
    Low      = 0b00,
    Medium   = 0b01,
    High     = 0b10,
    VeryHigh = 0b11
};

// Internal pull resistor (PUPDR field values).
enum class Pull : std::uint8_t {
    None = 0b00,
    Up   = 0b01,
    Down = 0b10
};

class Gpio {
public:
    // Construct a driver bound to an explicit register block.
    // Used directly by unit tests; used internally by at().
    explicit constexpr Gpio(GpioRegisters* regs) noexcept : regs_(regs) {}

    // Hardware factory: enable the port's RCC clock and bind to the real
    // peripheral. Only meaningful when running on the MCU.
    static Gpio at(Port port) noexcept;

    // Configure a single pin.
    void configure(Pin pin,
                   Mode mode,
                   Speed speed       = Speed::Low,
                   Pull pull         = Pull::None,
                   OutputType otype  = OutputType::PushPull) noexcept;

    // Drive an output pin high (true) or low (false) via BSRR (atomic).
    void write(Pin pin, bool high) noexcept;

    // Read the current input level of a pin from IDR.
    [[nodiscard]] bool read(Pin pin) const noexcept;

    // Flip an output pin's level based on its ODR state.
    void toggle(Pin pin) noexcept;

    // Raw register access (used by tests / advanced callers).
    [[nodiscard]] constexpr GpioRegisters* registers() const noexcept { return regs_; }

    // Map a Port enum to its peripheral base address.
    [[nodiscard]] static constexpr std::uintptr_t baseAddress(Port port) noexcept;

private:
    GpioRegisters* regs_;
};

constexpr std::uintptr_t Gpio::baseAddress(Port port) noexcept {
    switch (port) {
        case Port::A: return GPIOA_BASE;
        case Port::B: return GPIOB_BASE;
        case Port::C: return GPIOC_BASE;
        case Port::D: return GPIOD_BASE;
        case Port::E: return GPIOE_BASE;
        case Port::F: return GPIOF_BASE;
        case Port::G: return GPIOG_BASE;
        case Port::H: return GPIOH_BASE;
        case Port::I: return GPIOI_BASE;
    }
    return GPIOA_BASE;
}

}  // namespace stm32f407

#endif  // GPIO_HPP
