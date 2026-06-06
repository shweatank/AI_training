// ============================================================================
//  LED blinking demo - STM32F407 (embedded C++17)
//
//  Target board: STM32F4-Discovery
//    LD4 green  = PD12
//    LD3 orange = PD13
//    LD5 red    = PD14
//    LD6 blue   = PD15
//
//  Behaviour: the four user LEDs run as a rotating "chase" pattern, then all
//  blink together, forever.
// ============================================================================

#include "gpio.hpp"

namespace {

// Crude busy-wait delay. The CPU runs from the 16 MHz HSI at reset (no clock
// tree is configured here), so the loop count is only an approximation.
void delay_ms(std::uint32_t ms) noexcept {
    // ~16 MHz / (a few instructions per iteration). Tuned for a visible blink,
    // not for timing accuracy.
    constexpr std::uint32_t kIterPerMs = 1600U;
    for (std::uint32_t m = 0; m < ms; ++m) {
        for (volatile std::uint32_t i = 0; i < kIterPerMs; ++i) {
            // burn cycles
        }
    }
}

}  // namespace

int main() {
    using namespace stm32f407;

    // Enable GPIOD clock and bind a driver to the real peripheral.
    Gpio led = Gpio::at(Port::D);

    constexpr Pin kLeds[] = {Pin::P12, Pin::P13, Pin::P14, Pin::P15};

    // Configure each LED pin as a push-pull output.
    for (Pin p : kLeds) {
        led.configure(p, Mode::Output, Speed::Low, Pull::None, OutputType::PushPull);
        led.write(p, false);  // start off
    }

    while (true) {
        // 1) Chase: light each LED in turn.
        for (Pin p : kLeds) {
            led.write(p, true);
            delay_ms(150);
            led.write(p, false);
        }

        // 2) Blink all together twice.
        for (int blink = 0; blink < 2; ++blink) {
            for (Pin p : kLeds) {
                led.write(p, true);
            }
            delay_ms(250);
            for (Pin p : kLeds) {
                led.toggle(p);  // exercise toggle() too
            }
            delay_ms(250);
        }
    }

    return 0;
}
