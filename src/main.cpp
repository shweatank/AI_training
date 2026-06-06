// main.cpp — alternate two LEDs at a time on the STM32F407 Discovery board.
//
// Board LED map (STM32F407G-DISC1, all on port D):
//   PD12 = green, PD13 = orange, PD14 = red, PD15 = blue.
//
// Two pairs alternate: pair A (green+red) lights while pair B (orange+blue) is
// off, then they swap — two LEDs on at any moment, forever.
#include "blink.hpp"
#include "gpio.hpp"
#include "registers.hpp"

#include <array>
#include <cstdint>

namespace {

// Crude busy-wait. The chip boots on the 16 MHz HSI, so this is approximate;
// it is only meant to make the chase visible to the eye. `volatile` stops the
// optimizer from deleting the empty loop.
void delay(std::uint32_t loops) noexcept {
    for (volatile std::uint32_t i = 0; i < loops; ++i) {
        asm volatile("nop");
    }
}

} // namespace

int main() {
    using namespace stm32;

    // 1. Enable the GPIOD peripheral clock (it is off out of reset).
    rcc().AHB1ENR |= kRccAhb1En_GpioD;

    // 2. The four LEDs, grouped into two alternating pairs.
    const std::array<DigitalOut, 4> leds = {
        DigitalOut{gpiod(), 12},  // green  \ pair A
        DigitalOut{gpiod(), 14},  // red    /
        DigitalOut{gpiod(), 13},  // orange \ pair B
        DigitalOut{gpiod(), 15},  // blue   /
    };

    // 3. Configure all four as push-pull outputs, starting off.
    for (const auto& led : leds) { led.init(); led.clear(); }

    // Pair A = green+red, Pair B = orange+blue. Two LEDs lit per phase.
    constexpr std::uint32_t kPairA = (1U << 12) | (1U << 14);
    constexpr std::uint32_t kPairB = (1U << 13) | (1U << 15);
    const blink::Alternator alternator{gpiod(), kPairA, kPairB};

    constexpr std::uint32_t kStep = 800'000;  // digit separators are a C++14+ nicety

    // 4. Alternate the two pairs forever, swapping on each tick.
    bool phase = false;
    while (true) {
        alternator.apply(phase);
        delay(kStep);
        phase = !phase;
    }
}
