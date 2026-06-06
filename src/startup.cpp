/*
 * STM32F407 Startup Code
 * Provides Reset_Handler, vector table, and system call stubs
 */

#include <cstdint>

// ============ Linker-provided symbols ============
extern uint32_t _etext;   // End of .text (in FLASH)
extern uint32_t _sdata;   // Start of .data (in RAM)
extern uint32_t _edata;   // End of .data (in RAM)
extern uint32_t _sbss;    // Start of .bss (in RAM)
extern uint32_t _ebss;    // End of .bss (in RAM)
extern uint32_t _estack;  // End of stack (top of RAM)

// ============ Forward declarations ============
extern int main(void);
extern "C" void Reset_Handler(void);

// ============ Cortex-M4 Interrupt Handlers ============
extern "C" {
    // Default weak interrupt handler
    void Default_Handler(void) __attribute__((weak));
    void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
    void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
    
    // STM32F407 Peripheral Interrupt Handlers
    void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void PVD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TAMP_STAMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void RTC_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void RCC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void EXTI0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void EXTI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void EXTI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void EXTI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void EXTI4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA1_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA1_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA1_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA1_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA1_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA1_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA1_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void ADC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN1_TX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN1_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void EXTI9_5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM1_BRK_TIM9_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM1_UP_TIM10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void I2C1_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void I2C1_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void I2C2_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void I2C2_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void SPI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void SPI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void USART3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void RTC_Alarm_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void OTG_FS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void SPI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void UART4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void UART5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void TIM7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void ETH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void ETH_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN2_TX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN2_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN2_RX1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CAN2_SCE_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void OTG_FS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DMA2_Stream7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void USART6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void I2C3_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void I2C3_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void OTG_HS_EP1_OUT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void OTG_HS_EP1_IN_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void OTG_HS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void OTG_HS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void DCMI_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void CRYP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void HASH_RNG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void FPU_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
}

// ============ Vector Table (placed at 0x08000000) ============
typedef void (*VectorFunction)(void);

extern "C" {
    __attribute__((section(".vectors")))
    VectorFunction g_pfnVectors[] = {
        // Stack pointer
        reinterpret_cast<VectorFunction>(&_estack),
        
        // Cortex-M4 Core exceptions
        Reset_Handler,                          // Reset Handler
        NMI_Handler,                            // NMI Handler
        HardFault_Handler,                      // Hard Fault Handler
        MemManage_Handler,                      // Memory Management Handler
        BusFault_Handler,                       // Bus Fault Handler
        UsageFault_Handler,                     // Usage Fault Handler
        nullptr, nullptr, nullptr, nullptr,     // Reserved
        SVC_Handler,                            // SVC Handler
        DebugMon_Handler,                       // Debug Monitor Handler
        nullptr,                                // Reserved
        PendSV_Handler,                         // PendSV Handler
        SysTick_Handler,                        // SysTick Handler
        
        // STM32F407 Peripheral interrupts (82 total)
        WWDG_IRQHandler,
        PVD_IRQHandler,
        TAMP_STAMP_IRQHandler,
        RTC_WKUP_IRQHandler,
        FLASH_IRQHandler,
        RCC_IRQHandler,
        EXTI0_IRQHandler,
        EXTI1_IRQHandler,
        EXTI2_IRQHandler,
        EXTI3_IRQHandler,
        EXTI4_IRQHandler,
        DMA1_Stream0_IRQHandler,
        DMA1_Stream1_IRQHandler,
        DMA1_Stream2_IRQHandler,
        DMA1_Stream3_IRQHandler,
        DMA1_Stream4_IRQHandler,
        DMA1_Stream5_IRQHandler,
        DMA1_Stream6_IRQHandler,
        ADC_IRQHandler,
        CAN1_TX_IRQHandler,
        CAN1_RX0_IRQHandler,
        CAN1_RX1_IRQHandler,
        CAN1_SCE_IRQHandler,
        EXTI9_5_IRQHandler,
        TIM1_BRK_TIM9_IRQHandler,
        TIM1_UP_TIM10_IRQHandler,
        TIM1_TRG_COM_TIM11_IRQHandler,
        TIM1_CC_IRQHandler,
        TIM2_IRQHandler,
        TIM3_IRQHandler,
        TIM4_IRQHandler,
        I2C1_EV_IRQHandler,
        I2C1_ER_IRQHandler,
        I2C2_EV_IRQHandler,
        I2C2_ER_IRQHandler,
        SPI1_IRQHandler,
        SPI2_IRQHandler,
        USART1_IRQHandler,
        USART2_IRQHandler,
        USART3_IRQHandler,
        EXTI15_10_IRQHandler,
        RTC_Alarm_IRQHandler,
        OTG_FS_WKUP_IRQHandler,
        TIM5_IRQHandler,
        SPI3_IRQHandler,
        UART4_IRQHandler,
        UART5_IRQHandler,
        TIM6_DAC_IRQHandler,
        TIM7_IRQHandler,
        DMA2_Stream0_IRQHandler,
        DMA2_Stream1_IRQHandler,
        DMA2_Stream2_IRQHandler,
        DMA2_Stream3_IRQHandler,
        DMA2_Stream4_IRQHandler,
        ETH_IRQHandler,
        ETH_WKUP_IRQHandler,
        CAN2_TX_IRQHandler,
        CAN2_RX0_IRQHandler,
        CAN2_RX1_IRQHandler,
        CAN2_SCE_IRQHandler,
        OTG_FS_IRQHandler,
        DMA2_Stream5_IRQHandler,
        DMA2_Stream6_IRQHandler,
        DMA2_Stream7_IRQHandler,
        USART6_IRQHandler,
        I2C3_EV_IRQHandler,
        I2C3_ER_IRQHandler,
        OTG_HS_EP1_OUT_IRQHandler,
        OTG_HS_EP1_IN_IRQHandler,
        OTG_HS_WKUP_IRQHandler,
        OTG_HS_IRQHandler,
        DCMI_IRQHandler,
        CRYP_IRQHandler,
        HASH_RNG_IRQHandler,
        FPU_IRQHandler,
    };
}

// ============ Reset Handler ============
extern "C" void Reset_Handler(void) {
    // Copy initialized data from FLASH to RAM
    uint32_t* src = &_etext;
    uint32_t* dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }
    
    // Zero out uninitialized data (BSS)
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }
    
    // Call main()
    main();
    
    // Infinite loop if main returns
    while (1);
}

// ============ Default Exception Handler ============
extern "C" void Default_Handler(void) {
    // Infinite loop - fault occurred
    while (1);
}

// ============ System Call Stubs for newlib ============
extern "C" {
    // Exit function
    void _exit(int status) {
        (void)status;  // Unused
        while (1);     // Hang the system
    }
    
    // Write function (for printf)
    int _write(int file, const char* ptr, int len) {
        (void)file;    // Unused
        (void)ptr;     // Unused
        return len;    // Pretend we wrote everything
    }
    
    // Read function (for scanf)
    int _read(int file, char* ptr, int len) {
        (void)file;    // Unused
        (void)ptr;     // Unused
        (void)len;     // Unused
        return 0;      // Return EOF
    }
    
    // Seek function
    int _lseek(int file, int ptr, int dir) {
        (void)file;    // Unused
        (void)ptr;     // Unused
        (void)dir;     // Unused
        return 0;
    }
    
    // Close function
    int _close(int file) {
        (void)file;    // Unused
        return -1;     // Failure
    }
    
    // Fstat function
    int _fstat(int file, void* st) {
        (void)file;    // Unused
        (void)st;      // Unused
        return -1;     // Failure
    }
    
    // Isatty function
    int _isatty(int file) {
        (void)file;    // Unused
        return 1;      // True
    }
    
    // Sbrk function (for dynamic memory)
    extern uint32_t _ebss;
    void* _sbrk(int incr) {
        static uint32_t heap_end = (uint32_t)&_ebss;
        uint32_t prev_heap_end = heap_end;
        heap_end += incr;
        return (void*)prev_heap_end;
    }
}
