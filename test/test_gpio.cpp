// test_gpio.cpp — host-side unit tests for the DigitalOut GPIO driver.
//
// These run on your PC, not the board. DigitalOut takes a `Gpio&`, so we hand
// it a *fake* Gpio struct living in ordinary RAM and assert on exactly the
// register bits the driver writes. This isolates the driver logic from real
// hardware (no 0x40020C00 access needed).
//
// Build & run:  make -C test

#include "gpio.hpp"
#include "registers.hpp"
#include "test_framework.hpp"

// A fresh, zeroed fake port for each test.
static stm32::Gpio make_port() { return stm32::Gpio{}; }

// ---- Tests ----------------------------------------------------------------

// init() must set the 2-bit MODER field to Output (0b01) for the pin.
static void test_init_sets_output_mode() {
    stm32::Gpio port = make_port();
    stm32::DigitalOut led{port, 12};
    led.init();
    CHECK_EQ(port.MODER, (0b01u << 24));   // pin 12 -> MODER bits 24..25
}

// init() must clear only its own field, leaving neighbouring pins untouched.
static void test_init_preserves_other_pins() {
    stm32::Gpio port = make_port();
    port.MODER = 0xFFFFFFFFu;              // every pin pre-configured
    stm32::DigitalOut{port, 12}.init();
    CHECK_EQ(port.MODER, 0xFDFFFFFFu);     // bits 24,25 -> 0b01; rest stay 1
}

// init() must select push-pull, low speed, no pull for the pin's fields.
static void test_init_clears_otyper_ospeedr_pupdr() {
    stm32::Gpio port = make_port();
    port.OTYPER  = 0xFFFFFFFFu;
    port.OSPEEDR = 0xFFFFFFFFu;
    port.PUPDR   = 0xFFFFFFFFu;
    stm32::DigitalOut{port, 13}.init();    // pin 13 -> field bits 26..27
    CHECK_EQ(port.OTYPER,  0xFFFFFFFFu & ~(1u << 13));
    CHECK_EQ(port.OSPEEDR, 0xFFFFFFFFu & ~(0b11u << 26));
    CHECK_EQ(port.PUPDR,   0xFFFFFFFFu & ~(0b11u << 26));
}

// set() writes the low half of BSRR (atomic set).
static void test_set_uses_bsrr_low_half() {
    stm32::Gpio port = make_port();
    stm32::DigitalOut{port, 14}.set();
    CHECK_EQ(port.BSRR, (1u << 14));
}

// clear() writes the high half of BSRR (pin + 16 = atomic reset).
static void test_clear_uses_bsrr_high_half() {
    stm32::Gpio port = make_port();
    stm32::DigitalOut{port, 15}.clear();
    CHECK_EQ(port.BSRR, (1u << (15 + 16)));
}

// write(true)/write(false) map to set()/clear().
static void test_write_dispatches_set_clear() {
    stm32::Gpio port = make_port();
    stm32::DigitalOut led{port, 12};
    led.write(true);
    CHECK_EQ(port.BSRR, (1u << 12));
    port.BSRR = 0;
    led.write(false);
    CHECK_EQ(port.BSRR, (1u << (12 + 16)));
}

// toggle() flips just the pin's ODR bit.
static void test_toggle_flips_odr_bit() {
    stm32::Gpio port = make_port();
    stm32::DigitalOut led{port, 13};
    led.toggle();
    CHECK_EQ(port.ODR, (1u << 13));
    led.toggle();
    CHECK_EQ(port.ODR, 0u);
}

// read() reflects the pin's ODR bit, independent of other bits.
static void test_read_reports_odr_bit() {
    stm32::Gpio port = make_port();
    stm32::DigitalOut led{port, 14};
    CHECK(!led.read());
    port.ODR = (1u << 14) | (1u << 3);     // our pin + unrelated noise
    CHECK(led.read());
    port.ODR = (1u << 3);                  // only the noise
    CHECK(!led.read());
}

// Pin 15 exercises the top of the 32-bit registers (fields at bit 30).
static void test_highest_pin_field() {
    stm32::Gpio port = make_port();
    stm32::DigitalOut{port, 15}.init();
    CHECK_EQ(port.MODER, (0b01u << 30));
}

// The RCC clock-enable constant matches GPIOD (AHB1ENR bit 3).
static void test_rcc_gpiod_enable_bit() {
    CHECK_EQ(stm32::kRccAhb1En_GpioD, (1u << 3));
}

int main() {
    tf::banner("GPIO driver unit tests  (DigitalOut)");

    RUN(test_init_sets_output_mode,         "init() sets MODER field to Output (0b01) at pin*2");
    RUN(test_init_preserves_other_pins,     "init() touches only its own 2-bit field, neighbours intact");
    RUN(test_init_clears_otyper_ospeedr_pupdr, "init() selects push-pull, low speed, no pull-up/down");
    RUN(test_set_uses_bsrr_low_half,        "set() does atomic set via BSRR low half (1<<pin)");
    RUN(test_clear_uses_bsrr_high_half,     "clear() does atomic reset via BSRR high half (1<<(pin+16))");
    RUN(test_write_dispatches_set_clear,    "write(true/false) maps to set()/clear()");
    RUN(test_toggle_flips_odr_bit,          "toggle() flips exactly the pin's ODR bit");
    RUN(test_read_reports_odr_bit,          "read() reports the pin's ODR bit, ignoring others");
    RUN(test_highest_pin_field,             "pin 15 edge case: MODER field at bits 30..31");
    RUN(test_rcc_gpiod_enable_bit,          "RCC AHB1ENR GPIOD clock-enable is bit 3");

    return tf::summary("GPIO driver");
}
