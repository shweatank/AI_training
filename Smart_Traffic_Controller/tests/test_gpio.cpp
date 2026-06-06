/**
 * @file    test_gpio.cpp
 * @brief   Host unit tests for the GPIO driver (register-level, no hardware).
 *
 * A GpioRegs struct lives in ordinary host RAM and the driver is pointed at it.
 * We inspect the resulting register contents after each operation.
 */
#include "stm32f407_gpio.hpp"
#include "test_framework.hpp"

using namespace stm32f407;

namespace {
GpioRegs makeRegs() {
    GpioRegs r{}; // all fields zero-initialised
    return r;
}
} // namespace

TEST_CASE(gpio_configure_output_sets_moder) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    PinConfig cfg{};
    cfg.mode = PinMode::Output;
    gpio.configure(12, cfg);
    EXPECT_EQ(regs.MODER, (0x1U << 24)); // MODER[25:24] = 0b01
}

TEST_CASE(gpio_configure_alternate_and_af_register) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    PinConfig cfg{};
    cfg.mode = PinMode::Alternate;
    cfg.af   = AltFunction::AF7;
    gpio.configure(2, cfg);
    EXPECT_EQ(regs.MODER, (0x2U << 4));
    EXPECT_EQ(regs.AFR[0], (0x7U << 8));
}

TEST_CASE(gpio_configure_af_high_register_for_pin10) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    PinConfig cfg{};
    cfg.mode = PinMode::Alternate;
    cfg.af   = AltFunction::AF5;
    gpio.configure(10, cfg);
    EXPECT_EQ(regs.AFR[1], (0x5U << 8));
}

TEST_CASE(gpio_open_drain_sets_and_clears_otyper) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    PinConfig od{};
    od.mode  = PinMode::Output;
    od.otype = OutputType::OpenDrain;
    gpio.configure(5, od);
    EXPECT_EQ(regs.OTYPER, (1U << 5));
    PinConfig pp{};
    pp.mode  = PinMode::Output;
    pp.otype = OutputType::PushPull;
    gpio.configure(5, pp);
    EXPECT_EQ(regs.OTYPER, 0U);
}

TEST_CASE(gpio_pull_and_speed_fields) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    PinConfig cfg{};
    cfg.mode  = PinMode::Input;
    cfg.pull  = Pull::Up;
    cfg.speed = OutputSpeed::VeryHigh;
    gpio.configure(3, cfg);
    EXPECT_EQ(regs.PUPDR, (0x1U << 6));
    EXPECT_EQ(regs.OSPEEDR, (0x3U << 6));
}

TEST_CASE(gpio_set_uses_bsrr_low_half) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    gpio.set(12);
    EXPECT_EQ(regs.BSRR, (1U << 12));
}

TEST_CASE(gpio_reset_uses_bsrr_high_half) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    gpio.reset(12);
    EXPECT_EQ(regs.BSRR, (1U << (12 + 16)));
}

TEST_CASE(gpio_toggle_from_low_sets_bit) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    regs.ODR = 0;
    gpio.toggle(13);
    EXPECT_EQ(regs.BSRR, (1U << 13));
}

TEST_CASE(gpio_toggle_from_high_resets_bit) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    regs.ODR = (1U << 13);
    gpio.toggle(13);
    EXPECT_EQ(regs.BSRR, (1U << (13 + 16)));
}

TEST_CASE(gpio_read_reflects_idr) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    regs.IDR = (1U << 4);
    EXPECT_TRUE(gpio.read(4) == PinState::High);
    EXPECT_TRUE(gpio.read(5) == PinState::Low);
}

TEST_CASE(gpio_out_of_range_pin_is_ignored) {
    GpioRegs regs = makeRegs();
    Gpio gpio(&regs);
    PinConfig cfg{};
    cfg.mode = PinMode::Output;
    gpio.configure(16, cfg);
    gpio.set(99);
    EXPECT_EQ(regs.MODER, 0U);
    EXPECT_EQ(regs.BSRR, 0U);
}
