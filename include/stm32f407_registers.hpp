#ifndef STM32F407_REGISTERS_HPP
#define STM32F407_REGISTERS_HPP

#include <cstdint>

namespace stm32f407 {

// ========== Memory Addresses ==========
constexpr uint32_t GPIOA_BASE = 0x40020000;
constexpr uint32_t GPIOB_BASE = 0x40020400;
constexpr uint32_t GPIOC_BASE = 0x40020800;
constexpr uint32_t GPIOD_BASE = 0x40020C00;
constexpr uint32_t GPIOE_BASE = 0x40021000;
constexpr uint32_t GPIOF_BASE = 0x40021400;
constexpr uint32_t GPIOG_BASE = 0x40021800;
constexpr uint32_t GPIOH_BASE = 0x40021C00;
constexpr uint32_t GPIOI_BASE = 0x40022000;

constexpr uint32_t USART1_BASE = 0x40011000;
constexpr uint32_t USART2_BASE = 0x40004400;
constexpr uint32_t USART3_BASE = 0x40004800;
constexpr uint32_t UART4_BASE  = 0x40004C00;
constexpr uint32_t UART5_BASE  = 0x40005000;
constexpr uint32_t USART6_BASE = 0x40011400;

constexpr uint32_t RCC_BASE = 0x40023800;

// ========== RCC Register Structure ==========
struct RCCRegisters {
    volatile uint32_t CR;           // 0x00 - Clock Control Register
    volatile uint32_t PLLCFGR;      // 0x04 - PLL Configuration Register
    volatile uint32_t CFGR;         // 0x08 - Clock Configuration Register
    volatile uint32_t CIR;          // 0x0C - Clock Interrupt Register
    volatile uint32_t AHB1RSTR;     // 0x10 - AHB1 Peripheral Reset Register
    volatile uint32_t AHB2RSTR;     // 0x14 - AHB2 Peripheral Reset Register
    volatile uint32_t AHB3RSTR;     // 0x18 - AHB3 Peripheral Reset Register
    volatile uint32_t RESERVED0;    // 0x1C
    volatile uint32_t APB1RSTR;     // 0x20 - APB1 Peripheral Reset Register
    volatile uint32_t APB2RSTR;     // 0x24 - APB2 Peripheral Reset Register
    volatile uint32_t RESERVED1[2]; // 0x28-0x2F
    volatile uint32_t AHB1ENR;      // 0x30 - AHB1 Peripheral Clock Enable Register
    volatile uint32_t AHB2ENR;      // 0x34 - AHB2 Peripheral Clock Enable Register
    volatile uint32_t AHB3ENR;      // 0x38 - AHB3 Peripheral Clock Enable Register
    volatile uint32_t RESERVED2;    // 0x3C
    volatile uint32_t APB1ENR;      // 0x40 - APB1 Peripheral Clock Enable Register
    volatile uint32_t APB2ENR;      // 0x44 - APB2 Peripheral Clock Enable Register
};

// ========== GPIO Register Structure ==========
struct GPIORegisters {
    volatile uint32_t MODER;        // 0x00 - GPIO Port Mode Register
    volatile uint32_t OTYPER;       // 0x04 - GPIO Port Output Type Register
    volatile uint32_t OSPEEDR;      // 0x08 - GPIO Port Output Speed Register
    volatile uint32_t PUPDR;        // 0x0C - GPIO Port Pull-up/Pull-down Register
    volatile uint32_t IDR;          // 0x10 - GPIO Port Input Data Register
    volatile uint32_t ODR;          // 0x14 - GPIO Port Output Data Register
    volatile uint32_t BSRR;         // 0x18 - GPIO Port Bit Set/Reset Register
    volatile uint32_t LCKR;         // 0x1C - GPIO Port Configuration Lock Register
    volatile uint32_t AFRL;         // 0x20 - GPIO Alternate Function Low Register
    volatile uint32_t AFRH;         // 0x24 - GPIO Alternate Function High Register
};

// ========== UART Register Structure ==========
struct UARTRegisters {
    volatile uint32_t SR;           // 0x00 - Status Register
    volatile uint32_t DR;           // 0x04 - Data Register
    volatile uint32_t BRR;          // 0x08 - Baud Rate Register
    volatile uint32_t CR1;          // 0x0C - Control Register 1
    volatile uint32_t CR2;          // 0x10 - Control Register 2
    volatile uint32_t CR3;          // 0x14 - Control Register 3
    volatile uint32_t GTPR;         // 0x18 - Guard Time and Prescaler Register
};

// ========== RCC Clock Enable Bits ==========
// AHB1ENR
constexpr uint32_t RCC_AHB1ENR_GPIOAEN = (1 << 0);
constexpr uint32_t RCC_AHB1ENR_GPIOBEN = (1 << 1);
constexpr uint32_t RCC_AHB1ENR_GPIOCEN = (1 << 2);
constexpr uint32_t RCC_AHB1ENR_GPIODEN = (1 << 3);
constexpr uint32_t RCC_AHB1ENR_GPIOEEN = (1 << 4);
constexpr uint32_t RCC_AHB1ENR_GPIOFEN = (1 << 5);
constexpr uint32_t RCC_AHB1ENR_GPIOGEN = (1 << 6);
constexpr uint32_t RCC_AHB1ENR_GPIOHEN = (1 << 7);
constexpr uint32_t RCC_AHB1ENR_GPIOIEN = (1 << 8);

// APB1ENR
constexpr uint32_t RCC_APB1ENR_USART2EN = (1 << 17);
constexpr uint32_t RCC_APB1ENR_USART3EN = (1 << 18);
constexpr uint32_t RCC_APB1ENR_UART4EN  = (1 << 19);
constexpr uint32_t RCC_APB1ENR_UART5EN  = (1 << 20);

// APB2ENR
constexpr uint32_t RCC_APB2ENR_USART1EN = (1 << 4);
constexpr uint32_t RCC_APB2ENR_USART6EN = (1 << 5);

// ========== UART Status Register Bits ==========
constexpr uint32_t UART_SR_PE   = (1 << 0);  // Parity Error
constexpr uint32_t UART_SR_FE   = (1 << 1);  // Framing Error
constexpr uint32_t UART_SR_NE   = (1 << 2);  // Noise Error
constexpr uint32_t UART_SR_ORE  = (1 << 3);  // Overrun Error
constexpr uint32_t UART_SR_IDLE = (1 << 4);  // Idle Line Detected
constexpr uint32_t UART_SR_RXNE = (1 << 5);  // Read Data Register Not Empty
constexpr uint32_t UART_SR_TC   = (1 << 6);  // Transmission Complete
constexpr uint32_t UART_SR_TXE  = (1 << 7);  // Transmit Data Register Empty
constexpr uint32_t UART_SR_LBD  = (1 << 8);  // LIN Break Detection Flag
constexpr uint32_t UART_SR_CTS  = (1 << 9);  // CTS Flag

// ========== UART Control Register 1 Bits ==========
constexpr uint32_t UART_CR1_SBK   = (1 << 0);  // Send Break
constexpr uint32_t UART_CR1_RWU   = (1 << 1);  // Receiver Wakeup
constexpr uint32_t UART_CR1_RE    = (1 << 2);  // Receiver Enable
constexpr uint32_t UART_CR1_TE    = (1 << 3);  // Transmitter Enable
constexpr uint32_t UART_CR1_IDLEIE = (1 << 4); // IDLE Interrupt Enable
constexpr uint32_t UART_CR1_RXNEIE = (1 << 5); // RXNE Interrupt Enable
constexpr uint32_t UART_CR1_TCIE  = (1 << 6);  // Transmission Complete Interrupt Enable
constexpr uint32_t UART_CR1_TXEIE = (1 << 7);  // TXE Interrupt Enable
constexpr uint32_t UART_CR1_PEIE  = (1 << 8);  // PE Interrupt Enable
constexpr uint32_t UART_CR1_PS    = (1 << 9);  // Parity Selection
constexpr uint32_t UART_CR1_PCE   = (1 << 10); // Parity Control Enable
constexpr uint32_t UART_CR1_WAKE  = (1 << 11); // Wakeup Method
constexpr uint32_t UART_CR1_M     = (1 << 12); // Word Length
constexpr uint32_t UART_CR1_UE    = (1 << 13); // UART Enable

// ========== Helper Functions ==========
inline RCCRegisters* getRCCRegisters() {
    return reinterpret_cast<RCCRegisters*>(RCC_BASE);
}

inline GPIORegisters* getGPIORegisters(uintptr_t baseAddress) {
    return reinterpret_cast<GPIORegisters*>(baseAddress);
}

inline UARTRegisters* getUARTRegisters(uintptr_t baseAddress) {
    return reinterpret_cast<UARTRegisters*>(baseAddress);
}

} // namespace stm32f407

#endif // STM32F407_REGISTERS_HPP
