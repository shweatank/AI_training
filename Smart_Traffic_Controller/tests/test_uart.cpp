/**
 * @file    test_uart.cpp
 * @brief   Host unit tests for the USART driver (register-level, no hardware).
 *
 * The BRR formula is verified via the static helper. For the blocking
 * transmit/receive paths we pre-set the status flags (TXE/TC/RXNE) in the mock
 * status register so the polling loops fall straight through.
 */
#include "stm32f407_uart.hpp"
#include "test_framework.hpp"

using namespace stm32f407;

namespace {
UsartRegs makeReady() {
    UsartRegs r{};
    r.SR = usart_bits::kTXE | usart_bits::kTC | usart_bits::kRXNE;
    return r;
}
} // namespace

TEST_CASE(uart_brr_115200_at_16mhz) {
    EXPECT_EQ(Uart::computeBrr(16000000, 115200), 139U);
}

TEST_CASE(uart_brr_9600_at_16mhz) {
    EXPECT_EQ(Uart::computeBrr(16000000, 9600), 1667U);
}

TEST_CASE(uart_brr_zero_baud_is_safe) {
    EXPECT_EQ(Uart::computeBrr(16000000, 0), 0U);
}

TEST_CASE(uart_init_8n1_sets_control_bits) {
    UsartRegs regs{};
    Uart uart(&regs);
    UartConfig cfg{};
    cfg.baud_rate  = 115200;
    cfg.periph_clk = 16000000;
    uart.init(cfg);
    EXPECT_EQ(regs.BRR, 139U);
    EXPECT_TRUE((regs.CR1 & usart_bits::kUE) != 0);
    EXPECT_TRUE((regs.CR1 & usart_bits::kTE) != 0);
    EXPECT_TRUE((regs.CR1 & usart_bits::kRE) != 0);
    EXPECT_TRUE((regs.CR1 & usart_bits::kM) == 0);
    EXPECT_TRUE((regs.CR1 & usart_bits::kPCE) == 0);
    EXPECT_EQ(regs.CR2 & usart_bits::kStopMask, 0U);
}

TEST_CASE(uart_init_9bit_even_parity_two_stop) {
    UsartRegs regs{};
    Uart uart(&regs);
    UartConfig cfg{};
    cfg.word_length = WordLength::Bits9;
    cfg.parity      = Parity::Even;
    cfg.stop_bits   = StopBits::Two;
    uart.init(cfg);
    EXPECT_TRUE((regs.CR1 & usart_bits::kM) != 0);
    EXPECT_TRUE((regs.CR1 & usart_bits::kPCE) != 0);
    EXPECT_TRUE((regs.CR1 & usart_bits::kPS) == 0);
    EXPECT_EQ(regs.CR2 & usart_bits::kStopMask, (0x2U << usart_bits::kStopShift));
}

TEST_CASE(uart_init_odd_parity_sets_ps) {
    UsartRegs regs{};
    Uart uart(&regs);
    UartConfig cfg{};
    cfg.parity = Parity::Odd;
    uart.init(cfg);
    EXPECT_TRUE((regs.CR1 & usart_bits::kPCE) != 0);
    EXPECT_TRUE((regs.CR1 & usart_bits::kPS) != 0);
}

TEST_CASE(uart_set_rx_interrupt_toggles_rxneie) {
    UsartRegs regs{};
    Uart uart(&regs);
    uart.setRxInterrupt(true);
    EXPECT_TRUE((regs.CR1 & usart_bits::kRXNEIE) != 0);
    uart.setRxInterrupt(false);
    EXPECT_TRUE((regs.CR1 & usart_bits::kRXNEIE) == 0);
}

TEST_CASE(uart_write_byte_places_value_in_dr) {
    UsartRegs regs = makeReady();
    Uart uart(&regs);
    uart.writeByte(0xA5);
    EXPECT_EQ(regs.DR, 0xA5U);
}

TEST_CASE(uart_write_string_transmits_last_char) {
    UsartRegs regs = makeReady();
    Uart uart(&regs);
    uart.writeString("Hi");
    EXPECT_EQ(regs.DR, static_cast<std::uint32_t>('i'));
}

TEST_CASE(uart_write_string_null_is_safe) {
    UsartRegs regs = makeReady();
    Uart uart(&regs);
    uart.writeString(nullptr);
    EXPECT_EQ(regs.DR, 0U);
}

TEST_CASE(uart_read_byte_returns_dr) {
    UsartRegs regs = makeReady();
    regs.DR = 0x3C;
    Uart uart(&regs);
    EXPECT_EQ(uart.readByte(), 0x3CU);
}

TEST_CASE(uart_read_data_returns_low_byte_for_isr) {
    UsartRegs regs{};
    regs.DR = 0x1C7; // 9-bit frame; ISR path masks to 8 bits
    Uart uart(&regs);
    EXPECT_EQ(uart.readData(), 0xC7U);
}

TEST_CASE(uart_rx_ready_reflects_rxne) {
    UsartRegs regs{};
    Uart uart(&regs);
    EXPECT_FALSE(uart.rxReady());
    regs.SR = usart_bits::kRXNE;
    EXPECT_TRUE(uart.rxReady());
}
