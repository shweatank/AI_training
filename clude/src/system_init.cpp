/**
 * @file system_init.cpp
 * @brief System Initialization Implementation
 */

#include "system_init.h"
#include "stm32f407xx.h"

namespace system {

volatile uint32_t SystemInit::systick_counter_ = 0;
bool SystemInit::initialized_ = false;

bool SystemInit::initClocks() {
    configurePLL();
    configureFlash();
    configurePrescalers();
    return true;
}

bool SystemInit::initPeripherals() {
    auto* rcc = hal::RCC;
    if (!rcc) {
        return false;
    }

    // Enable GPIO clocks
    rcc->AHB1ENR |= hal::RCC_AHB1ENR_GPIODEN;

    // Enable UART1 clock
    rcc->APB2ENR |= hal::RCC_APB2ENR_USART1EN;

    return true;
}

bool SystemInit::initSysTick() {
    // Configure SysTick for 1 ms tick
    // SYSTICK_FREQ = 1000 Hz (1 ms)
    // SysTick is running at system clock / 8
    // For 168 MHz clock: 168000000 / 8 = 21000000 Hz
    // To get 1 ms tick: 21000000 / 1000 = 21000

    uint32_t reload_value = 21000 - 1;  // -1 because counter starts from 0

    // Set reload value
    volatile uint32_t* syst_rvr = reinterpret_cast<volatile uint32_t*>(0xE000E014);
    *syst_rvr = reload_value & 0x00FFFFFF;

    // Clear current value
    volatile uint32_t* syst_cvr = reinterpret_cast<volatile uint32_t*>(0xE000E018);
    *syst_cvr = 0;

    // Enable SysTick with interrupt
    volatile uint32_t* syst_csr = reinterpret_cast<volatile uint32_t*>(0xE000E010);
    *syst_csr = 0x07;  // ENABLE | TICKINT | CLKSOURCE

    systick_counter_ = 0;
    return true;
}

uint32_t SystemInit::getSysTick() {
    return systick_counter_;
}

void SystemInit::resetSysTick() {
    systick_counter_ = 0;
}

uint32_t SystemInit::getSystemClockFreq() {
    return SYSTEM_CLOCK_FREQ;
}

uint32_t SystemInit::getAPB1ClockFreq() {
    return APB1_CLOCK_FREQ;
}

uint32_t SystemInit::getAPB2ClockFreq() {
    return APB2_CLOCK_FREQ;
}

bool SystemInit::enableAllClocks() {
    return initPeripherals();
}

void SystemInit::configurePLL() {
    auto* rcc = hal::RCC;
    if (!rcc) {
        return;
    }

    // For STM32F407 @ 168 MHz with 8 MHz external clock:
    // PLLVCO = (HSE_VALUE / PLLM) * PLLN = (8 / 8) * 336 = 336 MHz
    // PLLCLK = PLLVCO / PLLP = 336 / 2 = 168 MHz
    // USB/SDIO/RNG clock = PLLVCO / PLLQ = 336 / 7 = 48 MHz

    uint32_t pllcfgr = 0;
    pllcfgr |= (8 << 0);      // PLLM = 8
    pllcfgr |= (336 << 6);    // PLLN = 336
    pllcfgr |= (0 << 16);     // PLLP = 2 (00 = 2)
    pllcfgr |= (7 << 24);     // PLLQ = 7
    pllcfgr |= (1 << 22);     // HSE as PLL source

    rcc->PLLCFGR = pllcfgr;

    // Enable PLL
    rcc->CR |= (1U << 24);  // PLLON

    // Wait for PLL to lock
    volatile uint32_t timeout = 0xFFFF;
    while ((!(rcc->CR & (1U << 25))) && timeout--) {
        __asm__("nop");
    }

    // Select PLL as system clock
    rcc->CFGR &= ~(0x3U);
    rcc->CFGR |= 0x2;  // SW = 10 (PLL)

    // Wait for clock switch
    timeout = 0xFFFF;
    while (((rcc->CFGR & 0xC) != 0x8) && timeout--) {
        __asm__("nop");
    }
}

void SystemInit::configureFlash() {
    // For 168 MHz operation, need 5 wait states
    volatile uint32_t* flash_acr = reinterpret_cast<volatile uint32_t*>(0x40023C00);
    *flash_acr |= 0x5;  // 5 wait states
    *flash_acr |= (1U << 8);  // Instruction cache enable
    *flash_acr |= (1U << 9);  // Data cache enable
    *flash_acr |= (1U << 10); // Prefetch enable
}

void SystemInit::configurePrescalers() {
    auto* rcc = hal::RCC;
    if (!rcc) {
        return;
    }

    // AHB prescaler = 1 (168 MHz)
    // APB1 prescaler = 4 (42 MHz) - Max 42 MHz
    // APB2 prescaler = 2 (84 MHz) - Max 84 MHz

    uint32_t cfgr = rcc->CFGR;
    cfgr &= ~(0xF << 4);   // Clear HPRE
    cfgr |= (0x0 << 4);    // HPRE = 0 (no division)

    cfgr &= ~(0x7 << 10);  // Clear PPRE1
    cfgr |= (0x5 << 10);   // PPRE1 = 101 (divide by 4)

    cfgr &= ~(0x7 << 13);  // Clear PPRE2
    cfgr |= (0x4 << 13);   // PPRE2 = 100 (divide by 2)

    rcc->CFGR = cfgr;
}

} // namespace system

// SysTick interrupt handler
extern "C" void SysTick_Handler() {
    system::SystemInit::systick_counter_++;
}
