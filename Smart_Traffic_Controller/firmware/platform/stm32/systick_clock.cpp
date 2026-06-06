/**
 * @file    systick_clock.cpp
 * @brief   Implementation of the SysTick millisecond clock + its ISR.
 */
#include "systick_clock.hpp"

#include "stm32f407_regs.hpp"

namespace stm32f407 {

namespace {
/// Free-running millisecond counter, advanced only by SysTick_Handler.
volatile std::uint32_t g_systick_ms = 0;

constexpr std::uint32_t kTicksPerSecond = 1000U; // 1 ms tick
} // namespace

bool SysTickClock::init(std::uint32_t core_clk_hz) noexcept {
    // Reload counts down core_clk/1000 cycles per millisecond. The counter is
    // 24-bit, so validate the value fits before enabling.
    const std::uint32_t reload = (core_clk_hz / kTicksPerSecond) - 1U;
    if (reload > systick_bits::kReloadMax) {
        return false; // core clock too high for a 1 ms tick on bare SysTick
    }

    SysTickRegs* st = SYSTICK();
    st->LOAD = reload;
    st->VAL  = 0; // clear current value and the COUNTFLAG
    st->CTRL = systick_bits::kClkSource | systick_bits::kTickInt |
               systick_bits::kEnable;
    return true;
}

std::uint32_t SysTickClock::nowMs() const noexcept {
    return g_systick_ms;
}

} // namespace stm32f407

// SysTick exception: strong override of the startup weak alias. Minimal work —
// just bump the millisecond counter.
extern "C" void SysTick_Handler() {
    ++stm32f407::g_systick_ms;
}
