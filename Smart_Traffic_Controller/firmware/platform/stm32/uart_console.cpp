/**
 * @file    uart_console.cpp
 * @brief   USART2 RX-interrupt glue for UartConsole + NVIC enable.
 */
#include "uart_console.hpp"

#include "stm32f407_regs.hpp"

namespace stm32f407 {

namespace {
/// The single console wired to USART2's interrupt (set by attachUsart2Isr).
UartConsole* g_usart2_console = nullptr;

/// Enable a peripheral IRQ line in the NVIC set-enable registers.
void nvicEnable(std::uint32_t irq_number) noexcept {
    NvicRegs* nvic = NVIC();
    nvic->ISER[irq_number >> 5] = (1U << (irq_number & 0x1FU));
}
} // namespace

void attachUsart2Isr(UartConsole* console) noexcept {
    g_usart2_console = console;
    if (console != nullptr) {
        nvicEnable(irq::kUsart2);
    }
}

} // namespace stm32f407

// USART2 global interrupt: strong override of the startup weak alias. Delegates
// to the attached console, which reads DR and buffers the byte. No parsing or
// transmission happens here.
extern "C" void USART2_IRQHandler() {
    if (stm32f407::g_usart2_console != nullptr) {
        stm32f407::g_usart2_console->onRxInterrupt();
    }
}
