// test_target.cpp — ON-TARGET (hardware-in-the-loop) unit tests.
//
// This runs ON the STM32F407, exercising the REAL GPIOD peripheral (not a RAM
// fake). Each test case is streamed to your PC terminal live via ARM
// semihosting — the firmware prints through the ST-LINK/OpenOCD debug channel,
// so no UART wiring is needed. Run it with:  make test-board
//
// Because it touches real silicon, set() actually lights an LED and we read the
// result back from ODR off the chip.
#include "blink.hpp"
#include "gpio.hpp"
#include "registers.hpp"

#include <cstdint>

namespace {

// ---- Minimal ARM semihosting (no newlib needed) --------------------------
// A `bkpt 0xAB` with op in r0 / arg in r1 traps to the debugger, which performs
// the host-side I/O. Only valid while a debugger (OpenOCD) is attached.
inline int semihost(int op, const void* arg) {
    register int op_r0 asm("r0") = op;
    register const void* arg_r1 asm("r1") = arg;
    asm volatile("bkpt #0xAB" : "+r"(op_r0) : "r"(arg_r1) : "memory", "cc");
    return op_r0;
}
inline void sh_write0(const char* s) { semihost(0x04, s); }            // SYS_WRITE0
inline void sh_exit()                { semihost(0x18, (void*)0x20026); } // SYS_EXIT (ApplicationExit)

// Tiny unsigned -> "0x........" (8 hex digits) formatter.
const char* hex32(std::uint32_t v, char* buf /* >=11 */) {
    buf[0] = '0'; buf[1] = 'x';
    for (int i = 0; i < 8; ++i) {
        const std::uint32_t nib = (v >> ((7 - i) * 4)) & 0xFu;
        buf[2 + i] = static_cast<char>(nib < 10 ? ('0' + nib) : ('a' + nib - 10));
    }
    buf[10] = '\0';
    return buf;
}
// Tiny unsigned -> decimal.
const char* dec(std::uint32_t v, char* buf /* >=11 */) {
    char tmp[11]; int n = 0;
    if (v == 0) tmp[n++] = '0';
    while (v) { tmp[n++] = static_cast<char>('0' + (v % 10)); v /= 10; }
    int j = 0;
    while (n) buf[j++] = tmp[--n];
    buf[j] = '\0';
    return buf;
}

int g_pass = 0, g_fail = 0, g_idx = 0;

// One on-target test case: compare a value read from real hardware to expected.
void check_eq(const char* name, const char* desc,
              std::uint32_t got, std::uint32_t want) {
    ++g_idx;
    char num[12];
    sh_write0("  ["); sh_write0(dec(static_cast<std::uint32_t>(g_idx), num)); sh_write0("] ");
    sh_write0(name); sh_write0("\r\n      -> "); sh_write0(desc); sh_write0("\r\n");
    if (got == want) {
        ++g_pass;
        sh_write0("      [PASS]\r\n\r\n");
    } else {
        ++g_fail;
        sh_write0("      [FAIL] got=");
        sh_write0(hex32(got, num));
        sh_write0(" want=");
        sh_write0(hex32(want, num));
        sh_write0("\r\n\r\n");
    }
}

unsigned popcount(std::uint32_t v) { return static_cast<unsigned>(__builtin_popcount(v)); }

} // namespace

int main() {
    using namespace stm32;

    sh_write0("\r\n================================================================\r\n");
    sh_write0("  STM32F407 ON-TARGET TESTS  (live GPIOD, via semihosting)\r\n");
    sh_write0("================================================================\r\n\r\n");

    // [1] Real RCC: enabling the clock must set AHB1ENR bit 3.
    rcc().AHB1ENR |= kRccAhb1En_GpioD;
    check_eq("rcc_gpiod_clock_enable",
             "GPIOD clock bit reads back set in RCC->AHB1ENR",
             (rcc().AHB1ENR >> 3) & 1u, 1u);

    DigitalOut green{gpiod(), 12}, orange{gpiod(), 13}, red{gpiod(), 14}, blue{gpiod(), 15};
    green.init(); orange.init(); red.init(); blue.init();
    green.clear(); orange.clear(); red.clear(); blue.clear();

    // [2] Real MODER: PD12 configured as output (field == 0b01).
    check_eq("init_sets_real_moder",
             "PD12 MODER field == Output(01) read from the chip",
             (gpiod().MODER >> 24) & 0b11u, 0b01u);

    // [3] set() actually lights PD12 (ODR bit goes high on hardware).
    green.set();
    check_eq("set_lights_pin_on_hw",
             "set() -> PD12 lit: GPIOD->ODR bit 12 == 1",
             (gpiod().ODR >> 12) & 1u, 1u);

    // [4] clear() turns it back off.
    green.clear();
    check_eq("clear_turns_pin_off_on_hw",
             "clear() -> PD12 off: GPIOD->ODR bit 12 == 0",
             (gpiod().ODR >> 12) & 1u, 0u);

    // [5] toggle() flips the live ODR bit.
    const std::uint32_t before = (gpiod().ODR >> 13) & 1u;
    orange.toggle();
    check_eq("toggle_flips_pin_on_hw",
             "toggle() flips PD13 ODR bit on the chip",
             (gpiod().ODR >> 13) & 1u, before ^ 1u);
    orange.clear();

    // ---- The actual blink behaviour, on real hardware ----
    constexpr std::uint32_t kA = (1u << 12) | (1u << 14);  // green + red
    constexpr std::uint32_t kB = (1u << 13) | (1u << 15);  // orange + blue
    constexpr std::uint32_t kMask = kA | kB;
    blink::Alternator alt{gpiod(), kA, kB};

    // [6] Phase 0 lights exactly pair A on the real port.
    alt.apply(false);
    check_eq("phase0_lights_pair_a_hw",
             "phase 0 -> only PD12+PD14 lit in live ODR",
             gpiod().ODR & kMask, kA);
    // [7] ...and that is exactly two LEDs.
    check_eq("phase0_exactly_two_leds_hw",
             "phase 0 -> popcount(live ODR) == 2",
             popcount(gpiod().ODR & kMask), 2u);

    // [8] Phase 1 lights exactly pair B.
    alt.apply(true);
    check_eq("phase1_lights_pair_b_hw",
             "phase 1 -> only PD13+PD15 lit in live ODR",
             gpiod().ODR & kMask, kB);
    // [9] ...exactly two LEDs.
    check_eq("phase1_exactly_two_leds_hw",
             "phase 1 -> popcount(live ODR) == 2",
             popcount(gpiod().ODR & kMask), 2u);

    // [10] Swapping back fully clears the previous pair (no stuck LED).
    alt.apply(false);
    check_eq("swap_clears_previous_pair_hw",
             "after swap -> pair B fully off in live ODR",
             gpiod().ODR & kB, 0u);

    // ---- Summary ----
    char num[12];
    sh_write0("----------------------------------------------------------------\r\n");
    sh_write0("  ON-TARGET SUMMARY: ");
    sh_write0(dec(static_cast<std::uint32_t>(g_pass), num)); sh_write0(" passed, ");
    sh_write0(dec(static_cast<std::uint32_t>(g_fail), num)); sh_write0(" failed, of ");
    sh_write0(dec(static_cast<std::uint32_t>(g_idx), num)); sh_write0(" test cases.\r\n");
    sh_write0(g_fail == 0 ? "  RESULT: ALL ON-TARGET TESTS PASSED\r\n"
                          : "  RESULT: FAILURES PRESENT\r\n");
    sh_write0("================================================================\r\n");

    // Visual cue on the board: all 4 LEDs on if everything passed.
    if (g_fail == 0) { green.set(); orange.set(); red.set(); blue.set(); }

    sh_exit();           // tell the debugger we're done
    while (true) { }     // and park here
}
