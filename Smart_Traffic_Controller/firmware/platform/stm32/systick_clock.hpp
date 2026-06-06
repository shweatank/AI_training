/**
 * @file    systick_clock.hpp
 * @brief   Cortex-M SysTick-backed millisecond clock (implements hal::IClock).
 *
 * Programs SysTick to fire every millisecond and increments a free-running
 * 32-bit counter in the interrupt. nowMs() simply returns that counter, so the
 * scheduler never blocks. The counter wraps at 2^32 ms (~49.7 days); all timing
 * comparisons elsewhere are wraparound-safe (see app::SoftTimer).
 *
 * The SysTick_Handler is defined in systick_clock.cpp as a strong symbol,
 * overriding the weak default handler in the startup file.
 */
#ifndef SYSTICK_CLOCK_HPP
#define SYSTICK_CLOCK_HPP

#include "hal/i_clock.hpp"
#include <cstdint>

namespace stm32f407 {

/// hal::IClock backed by the SysTick 1 kHz interrupt.
class SysTickClock final : public hal::IClock {
public:
    /**
     * @brief Configure SysTick for a 1 ms tick from the core clock.
     * @param core_clk_hz  processor clock feeding SysTick (Hz).
     * @return true if the 1 ms reload fits the 24-bit counter, false otherwise
     *         (in which case SysTick is left disabled).
     */
    bool init(std::uint32_t core_clk_hz) noexcept;

    std::uint32_t nowMs() const noexcept override;
};

} // namespace stm32f407

#endif // SYSTICK_CLOCK_HPP
