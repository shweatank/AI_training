/**
 * @file    stm32f407_rcc.hpp
 * @brief   Minimal Reset & Clock Control helper.
 *
 * Centralises peripheral clock gating so clock policy lives in one place rather
 * than scattered through the GPIO/UART drivers. Header-only; operates on an
 * injected RccRegs* so the logic is unit-testable on the host.
 */
#ifndef STM32F407_RCC_HPP
#define STM32F407_RCC_HPP

#include "stm32f407_regs.hpp"

namespace stm32f407 {

class Rcc {
public:
    explicit Rcc(RccRegs* regs) noexcept : regs_(regs) {}

    void enableGpioPortA() const noexcept { regs_->AHB1ENR |= rcc_bits::kGpioAEn; }
    void enableGpioPortB() const noexcept { regs_->AHB1ENR |= rcc_bits::kGpioBEn; }
    void enableGpioPortC() const noexcept { regs_->AHB1ENR |= rcc_bits::kGpioCEn; }
    void enableGpioPortD() const noexcept { regs_->AHB1ENR |= rcc_bits::kGpioDEn; }
    void enableGpioPortE() const noexcept { regs_->AHB1ENR |= rcc_bits::kGpioEEn; }

    void enableUsart1() const noexcept { regs_->APB2ENR |= rcc_bits::kUsart1En; }
    void enableUsart2() const noexcept { regs_->APB1ENR |= rcc_bits::kUsart2En; }
    void enableUsart3() const noexcept { regs_->APB1ENR |= rcc_bits::kUsart3En; }

private:
    RccRegs* regs_;
};

} // namespace stm32f407

#endif // STM32F407_RCC_HPP
