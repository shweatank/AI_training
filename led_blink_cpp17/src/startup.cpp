// ============================================================================
//  STM32F407 startup code (embedded C++17)
//  - Cortex-M4 vector table placed at 0x08000000
//  - Reset_Handler: init .data/.bss, run C++ static constructors, call main()
// ============================================================================

#include <cstdint>

// Linker-provided symbols (see stm32f407.ld).
extern std::uint32_t _sidata;  // .data load address in FLASH
extern std::uint32_t _sdata;   // .data start in RAM
extern std::uint32_t _edata;   // .data end in RAM
extern std::uint32_t _sbss;    // .bss start in RAM
extern std::uint32_t _ebss;    // .bss end in RAM
extern std::uint32_t _estack;  // top of stack

// C++ static constructor array.
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

extern int main();

extern "C" void Reset_Handler();
extern "C" void Default_Handler();

// Core + peripheral exception handlers: weak aliases to Default_Handler so the
// application can override any of them by simply defining a strong symbol.
#define WEAK_ALIAS __attribute__((weak, alias("Default_Handler")))

extern "C" {
    void NMI_Handler()        WEAK_ALIAS;
    void HardFault_Handler()  WEAK_ALIAS;
    void MemManage_Handler()  WEAK_ALIAS;
    void BusFault_Handler()   WEAK_ALIAS;
    void UsageFault_Handler() WEAK_ALIAS;
    void SVC_Handler()        WEAK_ALIAS;
    void DebugMon_Handler()   WEAK_ALIAS;
    void PendSV_Handler()     WEAK_ALIAS;
    void SysTick_Handler()    WEAK_ALIAS;
}

using VectorEntry = void (*)();

// Minimal vector table: 16 core entries + the first peripheral entry.
// That is sufficient for a polled LED-blink demo (no peripheral IRQs used).
__attribute__((section(".isr_vector"), used))
const VectorEntry g_vectors[] = {
    reinterpret_cast<VectorEntry>(&_estack),  // 0x00 initial stack pointer
    Reset_Handler,                            // 0x04 reset
    NMI_Handler,                              // 0x08
    HardFault_Handler,                        // 0x0C
    MemManage_Handler,                        // 0x10
    BusFault_Handler,                         // 0x14
    UsageFault_Handler,                       // 0x18
    nullptr, nullptr, nullptr, nullptr,       // 0x1C-0x28 reserved
    SVC_Handler,                              // 0x2C
    DebugMon_Handler,                         // 0x30
    nullptr,                                  // 0x34 reserved
    PendSV_Handler,                           // 0x38
    SysTick_Handler,                          // 0x3C
};

extern "C" void Reset_Handler() {
    // Enable the FPU before anything else. The firmware is built with the
    // hard-float ABI (-mfloat-abi=hard), but CP10/CP11 are disabled out of
    // reset; the first FP instruction would otherwise trigger a HardFault.
    // CPACR @ 0xE000ED88: set CP10 and CP11 to "full access" (0b11 each).
    volatile std::uint32_t* const CPACR =
        reinterpret_cast<volatile std::uint32_t*>(0xE000ED88U);
    *CPACR |= (0xFU << 20);
    __asm volatile("dsb");
    __asm volatile("isb");

    // Copy .data from FLASH to RAM.
    std::uint32_t* src = &_sidata;
    std::uint32_t* dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // Zero .bss.
    for (dst = &_sbss; dst < &_ebss; ) {
        *dst++ = 0;
    }

    // Run C++ static constructors.
    for (void (**ctor)() = __init_array_start; ctor < __init_array_end; ++ctor) {
        (*ctor)();
    }

    main();

    // main() should never return; trap if it does.
    while (true) {
    }
}

extern "C" void Default_Handler() {
    while (true) {
    }
}
