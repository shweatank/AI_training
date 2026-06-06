// test_blink.cpp — host-side unit tests for the "two LEDs at a time,
// alternating" pattern (blink::Alternator).
//
// A plain Gpio struct doesn't model the hardware side-effect where a BSRR write
// changes ODR (and BSRR reads back 0). `commit_bsrr()` below emulates exactly
// that, so after each apply() we can assert on ODR — i.e. on which LEDs are
// actually lit. That lets us prove "exactly two on, and they alternate".
//
// Build & run:  make -C test

#include "blink.hpp"
#include "registers.hpp"
#include "test_framework.hpp"

namespace {

// Emulate the GPIO hardware: apply a BSRR write to ODR, then clear BSRR.
//   low half  (bits  0..15) -> set   the matching ODR bits
//   high half (bits 16..31) -> reset the matching ODR bits  (set wins on clash)
void commit_bsrr(stm32::Gpio& p) {
    const std::uint32_t set_mask   = p.BSRR & 0xFFFFu;
    const std::uint32_t reset_mask = (p.BSRR >> 16) & 0xFFFFu;
    p.ODR &= ~reset_mask;
    p.ODR |= set_mask;
    p.BSRR = 0;
}

unsigned popcount(std::uint32_t v) { return static_cast<unsigned>(__builtin_popcount(v)); }

// Board pairing: A = green(12)+red(14), B = orange(13)+blue(15).
constexpr std::uint32_t kA = (1u << 12) | (1u << 14);
constexpr std::uint32_t kB = (1u << 13) | (1u << 15);

} // namespace

// ---- Tests ----------------------------------------------------------------

// The two groups must not share a pin, else "alternating" is meaningless.
static void test_groups_are_disjoint() {
    CHECK_EQ(kA & kB, 0u);
}

// apply() must be a single atomic BSRR write: set the on-group, reset the off-group.
static void test_apply_is_single_atomic_bsrr_write() {
    stm32::Gpio port{};
    blink::Alternator alt{port, kA, kB};
    alt.apply(false);
    CHECK_EQ(port.BSRR, kA | (kB << 16));   // phase 0: A on, B off
    port.BSRR = 0;
    alt.apply(true);
    CHECK_EQ(port.BSRR, kB | (kA << 16));   // phase 1: B on, A off
}

// Phase 0 lights exactly group A (two LEDs), nothing else.
static void test_phase0_lights_only_pair_a() {
    stm32::Gpio port{};
    blink::Alternator alt{port, kA, kB};
    alt.apply(false);
    commit_bsrr(port);
    CHECK_EQ(port.ODR, kA);
    CHECK_EQ(popcount(port.ODR), 2u);       // exactly two LEDs on
}

// Phase 1 lights exactly group B (two LEDs), nothing else.
static void test_phase1_lights_only_pair_b() {
    stm32::Gpio port{};
    blink::Alternator alt{port, kA, kB};
    alt.apply(true);
    commit_bsrr(port);
    CHECK_EQ(port.ODR, kB);
    CHECK_EQ(popcount(port.ODR), 2u);
}

// Swapping phase fully turns the previous pair off — no LED stays stuck on.
static void test_alternation_swaps_cleanly() {
    stm32::Gpio port{};
    blink::Alternator alt{port, kA, kB};

    alt.apply(false); commit_bsrr(port);
    CHECK_EQ(port.ODR, kA);

    alt.apply(true);  commit_bsrr(port);
    CHECK_EQ(port.ODR, kB);                 // A is now fully off
    CHECK_EQ(port.ODR & kA, 0u);

    alt.apply(false); commit_bsrr(port);
    CHECK_EQ(port.ODR, kA);                 // and back, B fully off
    CHECK_EQ(port.ODR & kB, 0u);
}

// Over many ticks: always exactly two LEDs, and always one whole group.
static void test_invariants_hold_over_a_cycle() {
    stm32::Gpio port{};
    blink::Alternator alt{port, kA, kB};
    bool phase = false;
    for (int tick = 0; tick < 8; ++tick) {
        alt.apply(phase);
        commit_bsrr(port);
        CHECK_EQ(popcount(port.ODR), 2u);                 // exactly two on
        CHECK(port.ODR == kA || port.ODR == kB);          // a complete group
        CHECK_EQ(port.ODR, phase ? kB : kA);              // the expected one
        phase = !phase;
    }
}

int main() {
    tf::banner("Blink pattern unit tests  (two LEDs alternating)");

    RUN(test_groups_are_disjoint,            "pairs A(green+red) and B(orange+blue) share no pin");
    RUN(test_apply_is_single_atomic_bsrr_write, "each phase = one glitch-free BSRR write (set on / reset off)");
    RUN(test_phase0_lights_only_pair_a,      "phase 0 lights exactly pair A — 2 LEDs, nothing else");
    RUN(test_phase1_lights_only_pair_b,      "phase 1 lights exactly pair B — 2 LEDs, nothing else");
    RUN(test_alternation_swaps_cleanly,      "swapping phase fully turns the previous pair off");
    RUN(test_invariants_hold_over_a_cycle,   "over 8 ticks: always exactly 2 on, always one whole group");

    return tf::summary("Blink pattern");
}
