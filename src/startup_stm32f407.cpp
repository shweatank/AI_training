// startup_stm32f407.cpp — C++ reset vector and minimal C-runtime bring-up.
//
// Responsibilities of Reset_Handler:
//   1. copy initialised data (.data) from flash to RAM,
//   2. zero-initialise .bss,
//   3. run C++ static constructors (.init_array),
//   4. call main(),
//   5. trap if main ever returns.
//
// Symbols (_sidata, _sdata, ...) are provided by the linker script.
#include <cstdint>

extern "C" {

// ---- Linker-provided symbols ---------------------------------------------
extern std::uint32_t _sidata;   // load address of .data in flash
extern std::uint32_t _sdata;    // start of .data in RAM
extern std::uint32_t _edata;    // end of .data in RAM
extern std::uint32_t _sbss;     // start of .bss
extern std::uint32_t _ebss;     // end of .bss
extern std::uint32_t _estack;   // top of stack (highest RAM address)

// C++ global constructor table.
using init_fn = void (*)();
extern init_fn __init_array_start;
extern init_fn __init_array_end;

int  main();
void Reset_Handler();
void Default_Handler();

// ---- Reset handler --------------------------------------------------------
void Reset_Handler() {
    // 1. Copy .data (flash -> RAM).
    std::uint32_t* src = &_sidata;
    std::uint32_t* dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // 2. Zero .bss.
    for (std::uint32_t* p = &_sbss; p < &_ebss; ++p) {
        *p = 0U;
    }

    // 3. Run static constructors.
    for (init_fn* fn = &__init_array_start; fn < &__init_array_end; ++fn) {
        (*fn)();
    }

    // 4. Hand off to the application.
    main();

    // 5. Should never return; if it does, spin.
    while (true) { /* trap */ }
}

// Any unhandled interrupt lands here.
void Default_Handler() {
    while (true) { /* trap */ }
}

// ---- Vector table ---------------------------------------------------------
// Only the core entries are wired up; every peripheral IRQ falls through to
// Default_Handler via the weak alias below. That is sufficient for polled
// blinking. Placed in its own section so the linker can put it at 0x08000000.
void NMI_Handler()        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler()  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler()  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler()   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler() __attribute__((weak, alias("Default_Handler")));
void SVC_Handler()        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler()   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler()     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler()    __attribute__((weak, alias("Default_Handler")));

using vector_t = void (*)();

__attribute__((section(".isr_vector"), used))
const vector_t g_vectors[] = {
    reinterpret_cast<vector_t>(&_estack), // 0x00: initial stack pointer
    Reset_Handler,                        // 0x04: reset
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    nullptr, nullptr, nullptr, nullptr,   // reserved
    SVC_Handler,
    DebugMon_Handler,
    nullptr,                              // reserved
    PendSV_Handler,
    SysTick_Handler,
    // Peripheral IRQs intentionally omitted — not needed for polled blinking.
};

} // extern "C"
