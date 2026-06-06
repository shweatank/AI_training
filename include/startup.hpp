#ifndef STARTUP_HPP
#define STARTUP_HPP

#include <cstdint>

namespace stm32f407 {

// System Clock Configuration
// Configure the system clock to 168 MHz using PLL
class SystemClock {
public:
    // Initialize system clock to 168 MHz
    // Uses HSE (8MHz external oscillator) as input
    static void init() {
        // Enable HSE
        RCC_CR |= (1 << 16);  // HSEON
        
        // Wait for HSE to be ready
        while (!(RCC_CR & (1 << 17)));  // HSERDY
        
        // Configure PLL
        // PLLVCO = (HSE_VALUE / PLL_M) * PLL_N = (8 / 8) * 336 = 336 MHz
        // SYSCLK = PLLVCO / PLL_P = 336 / 2 = 168 MHz
        // USB OTG FS, SDIO and RNG Clock = PLLVCO / PLL_Q = 336 / 7 = 48 MHz
        
        RCC_PLLCFGR = 0;
        RCC_PLLCFGR |= (0 << 22);      // PLL_SRC = HSE
        RCC_PLLCFGR |= (8 << 0);       // PLL_M = 8
        RCC_PLLCFGR |= (336 << 6);     // PLL_N = 336
        RCC_PLLCFGR |= (0 << 16);      // PLL_P = 2
        RCC_PLLCFGR |= (7 << 24);      // PLL_Q = 7
        
        // Enable PLL
        RCC_CR |= (1 << 24);  // PLLON
        
        // Wait for PLL to be ready
        while (!(RCC_CR & (1 << 25)));  // PLLRDY
        
        // Select PLL as system clock source
        RCC_CFGR |= (2 << 0);  // SW = PLL (10)
        
        // Wait until PLL is used as system clock source
        while ((RCC_CFGR & (3 << 2)) != (2 << 2));  // SWS = 10
    }

private:
    // RCC Register Addresses
    static constexpr uint32_t RCC_BASE_ADDR = 0x40023800;
    static volatile uint32_t& RCC_CR;      // 0x00
    static volatile uint32_t& RCC_PLLCFGR; // 0x04
    static volatile uint32_t& RCC_CFGR;    // 0x08
};

// Vector Table and Startup
extern "C" {
    // External symbols from linker script
    extern uint32_t _estack;
    extern uint32_t _sdata;
    extern uint32_t _edata;
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    
    // Function prototypes
    extern int main();
    
    // Exception handlers (weak symbols - can be overridden)
    void Default_Handler(void) __attribute__((weak));
    void Reset_Handler(void);
    void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
    
    // Interrupt handlers (peripheral)
    void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void PVD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TAMP_STAMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void RTC_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    // ... add more as needed
    
    // Default exception handler
    void Default_Handler(void) {
        while (1);
    }
    
    // Reset handler - entry point
    void Reset_Handler(void) {
        // Copy initialized data from flash to RAM
        uint32_t* src = &_etext;
        uint32_t* dest = &_sdata;
        while (dest < &_edata) {
            *dest++ = *src++;
        }
        
        // Zero out BSS section
        for (uint32_t* bss = &_sbss; bss < &_ebss; bss++) {
            *bss = 0;
        }
        
        // Call main
        main();
        
        // Hang if main returns
        while (1);
    }
}

#endif // STARTUP_HPP
