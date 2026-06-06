// blink.hpp — the "two LEDs at a time, alternating" pattern, isolated from
// main() so it can be unit-tested on the host.
//
// Two groups of LEDs (each a bitmask of pins on one port) take turns: one group
// is lit while the other is dark, then they swap. Each phase is applied in ONE
// atomic BSRR write, so both LEDs of a group switch together with no flicker.
#pragma once

#include "registers.hpp"
#include <cstdint>

namespace blink {

class Alternator {
public:
    constexpr Alternator(stm32::Gpio& port,
                         std::uint32_t group_a,
                         std::uint32_t group_b) noexcept
        : port_{port}, group_a_{group_a}, group_b_{group_b} {}

    // phase=false -> group A on, group B off.  phase=true -> the reverse.
    // BSRR low half sets, high half (<<16) resets; a single write switches the
    // whole pattern at once.
    void apply(bool phase) const noexcept {
        const std::uint32_t on  = phase ? group_b_ : group_a_;
        const std::uint32_t off = phase ? group_a_ : group_b_;
        port_.BSRR = on | (off << 16);
    }

    constexpr std::uint32_t group_a() const noexcept { return group_a_; }
    constexpr std::uint32_t group_b() const noexcept { return group_b_; }

private:
    stm32::Gpio& port_;
    std::uint32_t group_a_;
    std::uint32_t group_b_;
};

} // namespace blink
