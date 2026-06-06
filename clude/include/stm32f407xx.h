/**
 * @file stm32f407xx.h
 * @brief STM32F407 Register Definitions
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef STM32F407XX_H
#define STM32F407XX_H

#include <cstdint>

namespace hal {

// Base addresses
constexpr uint32_t PERIPH_BASE = 0x40000000U;
constexpr uint32_t AHB1_BASE = (PERIPH_BASE + 0x20000U);
constexpr uint32_t RCC_BASE = (AHB1_BASE + 0x3800U);
constexpr uint32_t GPIOD_BASE = (AHB1_BASE + 0x0C00U);
constexpr uint32_t UART1_BASE = 0x40011000U;

// LED Configuration
constexpr uint8_t LED_COUNT = 4;
constexpr uint8_t LED_GREEN = 12;    // PD12
constexpr uint8_t LED_ORANGE = 13;   // PD13
constexpr uint8_t LED_RED = 14;      // PD14
constexpr uint8_t LED_BLUE = 15;     // PD15

// Register structures
struct GPIOx_TypeDef {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRRL;
    volatile uint32_t BSRRH;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
};

struct RCC_TypeDef {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    volatile uint32_t RESERVED0;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    volatile uint32_t RESERVED2;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
};

struct USART_TypeDef {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
};

// Peripheral pointers (using bit_cast in C++20, but reinterpret_cast for C++17)
extern GPIOx_TypeDef* const GPIOD;
extern RCC_TypeDef* const RCC;
extern USART_TypeDef* const USART1;

// Inline definitions for constexpr compatibility
namespace {
    constexpr uintptr_t GPIOD_ADDR = GPIOD_BASE;
    constexpr uintptr_t RCC_ADDR = RCC_BASE;
    constexpr uintptr_t USART1_ADDR = UART1_BASE;
}

// Get pointers at runtime
inline GPIOx_TypeDef* getGPIOD() {
    return reinterpret_cast<GPIOx_TypeDef*>(GPIOD_BASE);
}

inline RCC_TypeDef* getRCC() {
    return reinterpret_cast<RCC_TypeDef*>(RCC_BASE);
}

inline USART_TypeDef* getUSART1() {
    return reinterpret_cast<USART_TypeDef*>(UART1_BASE);
}

// RCC AHB1ENR register bits
constexpr uint32_t RCC_AHB1ENR_GPIODEN = (1U << 3);

// RCC APB2ENR register bits
constexpr uint32_t RCC_APB2ENR_USART1EN = (1U << 4);

// USART CR1 bits
constexpr uint32_t USART_CR1_UE = (1U << 13);
constexpr uint32_t USART_CR1_M = (1U << 12);
constexpr uint32_t USART_CR1_PCE = (1U << 10);
constexpr uint32_t USART_CR1_PARITY = (1U << 9);
constexpr uint32_t USART_CR1_TXEIE = (1U << 7);
constexpr uint32_t USART_CR1_RXNEIE = (1U << 5);
constexpr uint32_t USART_CR1_TE = (1U << 3);
constexpr uint32_t USART_CR1_RE = (1U << 2);

// USART SR bits
constexpr uint32_t USART_SR_TC = (1U << 6);
constexpr uint32_t USART_SR_RXNE = (1U << 5);
constexpr uint32_t USART_SR_TXE = (1U << 7);

} // namespace hal

#endif // STM32F407XX_H
