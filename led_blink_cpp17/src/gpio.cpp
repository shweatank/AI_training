// ============================================================================
//  STM32F407 GPIO driver implementation - embedded C++17
// ============================================================================

#include "gpio.hpp"

namespace stm32f407 {
namespace {

// Enable the AHB1 clock for the given port (hardware only).
void enablePortClock(Port port) noexcept {
    RccRegisters* r = rcc();
    switch (port) {
        case Port::A: r->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; break;
        case Port::B: r->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; break;
        case Port::C: r->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; break;
        case Port::D: r->AHB1ENR |= RCC_AHB1ENR_GPIODEN; break;
        case Port::E: r->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; break;
        case Port::F: r->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; break;
        case Port::G: r->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; break;
        case Port::H: r->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; break;
        case Port::I: r->AHB1ENR |= RCC_AHB1ENR_GPIOIEN; break;
    }
    // Short read-back delay so the clock is stable before first access.
    volatile std::uint32_t dummy = r->AHB1ENR;
    (void)dummy;
}

}  // namespace

Gpio Gpio::at(Port port) noexcept {
    enablePortClock(port);
    return Gpio(reinterpret_cast<GpioRegisters*>(baseAddress(port)));
}

void Gpio::configure(Pin pin, Mode mode, Speed speed, Pull pull,
                     OutputType otype) noexcept {
    const std::uint32_t n = static_cast<std::uint32_t>(pin);
    const std::uint32_t two = 2U * n;

    // MODER: 2 bits per pin.
    regs_->MODER = (regs_->MODER & ~(0x3U << two)) |
                   (static_cast<std::uint32_t>(mode) << two);

    // OTYPER: 1 bit per pin.
    regs_->OTYPER = (regs_->OTYPER & ~(0x1U << n)) |
                    (static_cast<std::uint32_t>(otype) << n);

    // OSPEEDR: 2 bits per pin.
    regs_->OSPEEDR = (regs_->OSPEEDR & ~(0x3U << two)) |
                     (static_cast<std::uint32_t>(speed) << two);

    // PUPDR: 2 bits per pin.
    regs_->PUPDR = (regs_->PUPDR & ~(0x3U << two)) |
                   (static_cast<std::uint32_t>(pull) << two);
}

void Gpio::write(Pin pin, bool high) noexcept {
    const std::uint32_t n = static_cast<std::uint32_t>(pin);
    // BSRR: lower 16 bits set, upper 16 bits reset. Atomic, no read-modify-write.
    regs_->BSRR = high ? (1U << n) : (1U << (n + 16U));
}

bool Gpio::read(Pin pin) const noexcept {
    const std::uint32_t n = static_cast<std::uint32_t>(pin);
    return (regs_->IDR >> n) & 0x1U;
}

void Gpio::toggle(Pin pin) noexcept {
    const std::uint32_t n = static_cast<std::uint32_t>(pin);
    const bool currentlyHigh = (regs_->ODR >> n) & 0x1U;
    write(pin, !currentlyHigh);
}

}  // namespace stm32f407
