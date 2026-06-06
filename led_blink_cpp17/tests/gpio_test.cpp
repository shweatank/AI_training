// ============================================================================
//  Host unit tests for the STM32F407 GPIO driver (GoogleTest).
//
//  The driver takes a GpioRegisters* at construction. Here we hand it a plain
//  GpioRegisters object living in host RAM, so we can verify exactly which
//  register bits the driver sets/clears - no hardware required.
// ============================================================================

#include <gtest/gtest.h>

#include "gpio.hpp"

using namespace stm32f407;

namespace {

// Test fixture providing a zeroed fake register block per test.
class GpioTest : public ::testing::Test {
protected:
    GpioRegisters regs{};        // value-initialized => all fields 0
    Gpio gpio{&regs};
};

}  // namespace

// ---------------------------------------------------------------------------
//  Enum encodings must match the hardware field values (RM0090).
// ---------------------------------------------------------------------------
TEST(GpioEnums, ModeEncoding) {
    EXPECT_EQ(static_cast<uint8_t>(Mode::Input), 0b00);
    EXPECT_EQ(static_cast<uint8_t>(Mode::Output), 0b01);
    EXPECT_EQ(static_cast<uint8_t>(Mode::Alternate), 0b10);
    EXPECT_EQ(static_cast<uint8_t>(Mode::Analog), 0b11);
}

TEST(GpioEnums, SpeedPullTypeEncoding) {
    EXPECT_EQ(static_cast<uint8_t>(Speed::Low), 0b00);
    EXPECT_EQ(static_cast<uint8_t>(Speed::VeryHigh), 0b11);
    EXPECT_EQ(static_cast<uint8_t>(Pull::Up), 0b01);
    EXPECT_EQ(static_cast<uint8_t>(Pull::Down), 0b10);
    EXPECT_EQ(static_cast<uint8_t>(OutputType::OpenDrain), 1);
}

TEST(GpioEnums, PinAndPortValues) {
    EXPECT_EQ(static_cast<uint8_t>(Pin::P12), 12);
    EXPECT_EQ(static_cast<uint8_t>(Port::D), 3);
}

// ---------------------------------------------------------------------------
//  Base address mapping is a constexpr lookup -> verify at compile time.
// ---------------------------------------------------------------------------
TEST(GpioBaseAddress, PortMapping) {
    static_assert(Gpio::baseAddress(Port::A) == 0x40020000UL);
    static_assert(Gpio::baseAddress(Port::D) == 0x40020C00UL);
    static_assert(Gpio::baseAddress(Port::I) == 0x40022000UL);
    EXPECT_EQ(Gpio::baseAddress(Port::B), 0x40020400UL);
}

// ---------------------------------------------------------------------------
//  configure()
// ---------------------------------------------------------------------------
TEST_F(GpioTest, ConfigureSetsModerOutput) {
    gpio.configure(Pin::P12, Mode::Output);
    // Pin 12 => MODER bits [25:24] = 0b01
    EXPECT_EQ((regs.MODER >> 24) & 0x3U, 0b01U);
    // Other pins untouched.
    EXPECT_EQ(regs.MODER & ~(0x3U << 24), 0U);
}

TEST_F(GpioTest, ConfigureSetsAllFields) {
    gpio.configure(Pin::P5, Mode::Alternate, Speed::VeryHigh, Pull::Up,
                   OutputType::OpenDrain);
    EXPECT_EQ((regs.MODER   >> (2 * 5)) & 0x3U, static_cast<uint32_t>(Mode::Alternate));
    EXPECT_EQ((regs.OSPEEDR >> (2 * 5)) & 0x3U, static_cast<uint32_t>(Speed::VeryHigh));
    EXPECT_EQ((regs.PUPDR   >> (2 * 5)) & 0x3U, static_cast<uint32_t>(Pull::Up));
    EXPECT_EQ((regs.OTYPER  >> 5) & 0x1U, static_cast<uint32_t>(OutputType::OpenDrain));
}

TEST_F(GpioTest, ConfigureClearsPreviousModeBits) {
    // Pre-load all-ones into MODER for pin 12, then reconfigure as Output.
    regs.MODER = 0x3U << 24;
    gpio.configure(Pin::P12, Mode::Output);
    EXPECT_EQ((regs.MODER >> 24) & 0x3U, 0b01U);  // 0b11 -> 0b01, not OR'd to 0b11
}

TEST_F(GpioTest, ConfigureIsIndependentPerPin) {
    gpio.configure(Pin::P0, Mode::Output);
    gpio.configure(Pin::P15, Mode::Analog);
    EXPECT_EQ((regs.MODER >> 0) & 0x3U, static_cast<uint32_t>(Mode::Output));
    EXPECT_EQ((regs.MODER >> 30) & 0x3U, static_cast<uint32_t>(Mode::Analog));
}

// ---------------------------------------------------------------------------
//  write() -> BSRR (set in low half, reset in high half)
// ---------------------------------------------------------------------------
TEST_F(GpioTest, WriteHighSetsLowBsrrBit) {
    gpio.write(Pin::P12, true);
    EXPECT_EQ(regs.BSRR, 1U << 12);
}

TEST_F(GpioTest, WriteLowSetsHighBsrrBit) {
    gpio.write(Pin::P12, false);
    EXPECT_EQ(regs.BSRR, 1U << (12 + 16));
}

// ---------------------------------------------------------------------------
//  read() <- IDR
// ---------------------------------------------------------------------------
TEST_F(GpioTest, ReadReflectsIdr) {
    regs.IDR = 1U << 7;
    EXPECT_TRUE(gpio.read(Pin::P7));
    EXPECT_FALSE(gpio.read(Pin::P6));
}

// ---------------------------------------------------------------------------
//  toggle() inspects ODR and writes the opposite level via BSRR
// ---------------------------------------------------------------------------
TEST_F(GpioTest, ToggleFromLowDrivesHigh) {
    regs.ODR = 0;                 // pin 13 currently low
    gpio.toggle(Pin::P13);
    EXPECT_EQ(regs.BSRR, 1U << 13);  // set bit -> drive high
}

TEST_F(GpioTest, ToggleFromHighDrivesLow) {
    regs.ODR = 1U << 13;          // pin 13 currently high
    gpio.toggle(Pin::P13);
    EXPECT_EQ(regs.BSRR, 1U << (13 + 16));  // reset bit -> drive low
}

// ---------------------------------------------------------------------------
//  Sanity: a full configure + drive sequence on the Discovery LEDs.
// ---------------------------------------------------------------------------
TEST_F(GpioTest, DiscoveryLedSequence) {
    constexpr Pin leds[] = {Pin::P12, Pin::P13, Pin::P14, Pin::P15};
    for (Pin p : leds) {
        gpio.configure(p, Mode::Output);
    }
    // All four pins configured as output (0b01) in MODER bits [31:24].
    EXPECT_EQ((regs.MODER >> 24) & 0xFFU, 0b01010101U);

    gpio.write(Pin::P14, true);
    EXPECT_EQ(regs.BSRR, 1U << 14);
}
