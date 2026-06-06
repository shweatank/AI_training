/**
 * @file    startup_stm32f407.cpp
 * @brief   Minimal C++ reset handler and vector table for the STM32F407.
 *
 * Reset_Handler responsibilities:
 *   1. Copy initialised data (.data) from flash to RAM.
 *   2. Zero the .bss section.
 *   3. Run C++ static constructors (init_array).
 *   4. Call main().
 *
 * The vector table covers the 16 Cortex-M4 system exceptions plus external IRQs
 * up to USART2 (IRQ 38), which the traffic controller uses for buffered RX.
 * SysTick_Handler and USART2_IRQHandler are declared weak here and overridden
 * by strong definitions in the platform drivers.
 *
 * Linker symbols (_sidata, _sdata, …) come from the linker script.
 */
#include <cstdint>

// --- Linker-provided symbols ---------------------------------------------
extern std::uint32_t _sidata; // start of .data init values in flash
extern std::uint32_t _sdata;  // start of .data in RAM
extern std::uint32_t _edata;  // end of .data in RAM
extern std::uint32_t _sbss;   // start of .bss
extern std::uint32_t _ebss;   // end of .bss
extern std::uint32_t _estack; // top of stack

// C++ global constructor table.
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

extern "C" int  main();
extern "C" void Reset_Handler();
extern "C" void Default_Handler();

/// Run all registered static constructors.
static void callStaticConstructors() {
    for (void (**ctor)() = __init_array_start; ctor != __init_array_end; ++ctor) {
        (*ctor)();
    }
}

extern "C" void Reset_Handler() {
    // 1. Copy .data from flash to RAM.
    std::uint32_t* src = &_sidata;
    std::uint32_t* dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // 2. Zero .bss.
    for (std::uint32_t* b = &_sbss; b < &_ebss; ++b) {
        *b = 0;
    }

    // 3. Static constructors.
    callStaticConstructors();

    // 4. Application entry.
    main();

    // main() should never return; trap if it does.
    while (true) {
    }
}

/// Catch-all handler for unimplemented interrupts.
extern "C" void Default_Handler() {
    while (true) {
    }
}

// --- Weak aliases ---------------------------------------------------------
#define WEAK_ALIAS __attribute__((weak, alias("Default_Handler")))

// Core system exceptions.
extern "C" void NMI_Handler()        WEAK_ALIAS;
extern "C" void HardFault_Handler()  WEAK_ALIAS;
extern "C" void MemManage_Handler()  WEAK_ALIAS;
extern "C" void BusFault_Handler()   WEAK_ALIAS;
extern "C" void UsageFault_Handler() WEAK_ALIAS;
extern "C" void SVC_Handler()        WEAK_ALIAS;
extern "C" void DebugMon_Handler()   WEAK_ALIAS;
extern "C" void PendSV_Handler()     WEAK_ALIAS;
extern "C" void SysTick_Handler()    WEAK_ALIAS; // overridden by systick_clock.cpp

// External interrupts actually used (overridden by platform drivers).
extern "C" void USART2_IRQHandler()  WEAK_ALIAS; // overridden by uart_console.cpp

using handler_t = void (*)();

/**
 * Vector table: 16 system exceptions followed by external IRQ entries. We
 * populate through IRQ 38 (USART2); the slot at index 16+38 = 54 carries the
 * USART2 handler, and every other external slot defaults to Default_Handler.
 */
__attribute__((section(".isr_vector"), used))
const handler_t g_vector_table[] = {
    reinterpret_cast<handler_t>(&_estack), // 0x00 initial stack pointer
    Reset_Handler,                         // 0x04 reset
    NMI_Handler,                           // 0x08
    HardFault_Handler,                     // 0x0C
    MemManage_Handler,                     // 0x10
    BusFault_Handler,                      // 0x14
    UsageFault_Handler,                    // 0x18
    nullptr, nullptr, nullptr, nullptr,    // 0x1C-0x28 reserved
    SVC_Handler,                           // 0x2C
    DebugMon_Handler,                      // 0x30
    nullptr,                               // 0x34 reserved
    PendSV_Handler,                        // 0x38
    SysTick_Handler,                       // 0x3C

    // --- External interrupts (IRQ0 .. IRQ38) ---
    Default_Handler, // IRQ0  WWDG
    Default_Handler, // IRQ1  PVD
    Default_Handler, // IRQ2  TAMP_STAMP
    Default_Handler, // IRQ3  RTC_WKUP
    Default_Handler, // IRQ4  FLASH
    Default_Handler, // IRQ5  RCC
    Default_Handler, // IRQ6  EXTI0
    Default_Handler, // IRQ7  EXTI1
    Default_Handler, // IRQ8  EXTI2
    Default_Handler, // IRQ9  EXTI3
    Default_Handler, // IRQ10 EXTI4
    Default_Handler, // IRQ11 DMA1_Stream0
    Default_Handler, // IRQ12 DMA1_Stream1
    Default_Handler, // IRQ13 DMA1_Stream2
    Default_Handler, // IRQ14 DMA1_Stream3
    Default_Handler, // IRQ15 DMA1_Stream4
    Default_Handler, // IRQ16 DMA1_Stream5
    Default_Handler, // IRQ17 DMA1_Stream6
    Default_Handler, // IRQ18 ADC
    Default_Handler, // IRQ19 CAN1_TX
    Default_Handler, // IRQ20 CAN1_RX0
    Default_Handler, // IRQ21 CAN1_RX1
    Default_Handler, // IRQ22 CAN1_SCE
    Default_Handler, // IRQ23 EXTI9_5
    Default_Handler, // IRQ24 TIM1_BRK_TIM9
    Default_Handler, // IRQ25 TIM1_UP_TIM10
    Default_Handler, // IRQ26 TIM1_TRG_COM_TIM11
    Default_Handler, // IRQ27 TIM1_CC
    Default_Handler, // IRQ28 TIM2
    Default_Handler, // IRQ29 TIM3
    Default_Handler, // IRQ30 TIM4
    Default_Handler, // IRQ31 I2C1_EV
    Default_Handler, // IRQ32 I2C1_ER
    Default_Handler, // IRQ33 I2C2_EV
    Default_Handler, // IRQ34 I2C2_ER
    Default_Handler, // IRQ35 SPI1
    Default_Handler, // IRQ36 SPI2
    Default_Handler, // IRQ37 USART1
    USART2_IRQHandler, // IRQ38 USART2  <-- console RX
};
