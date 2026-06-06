#ifndef STM32F407_REGISTERS_HPP
#define STM32F407_REGISTERS_HPP

// ============================================================================
//  STM32F407 register definitions (GPIO + RCC subset)
//  Embedded C++17 - header only, no dynamic allocation, no exceptions.
// ============================================================================

#include <cstdint>

namespace stm32f407 {

// ---------------------------------------------------------------------------
//  Peripheral base addresses (Reference Manual RM0090, memory map)
// ---------------------------------------------------------------------------
inline constexpr std::uintptr_t GPIOA_BASE = 0x40020000UL;
inline constexpr std::uintptr_t GPIOB_BASE = 0x40020400UL;
inline constexpr std::uintptr_t GPIOC_BASE = 0x40020800UL;
inline constexpr std::uintptr_t GPIOD_BASE = 0x40020C00UL;
inline constexpr std::uintptr_t GPIOE_BASE = 0x40021000UL;
inline constexpr std::uintptr_t GPIOF_BASE = 0x40021400UL;
inline constexpr std::uintptr_t GPIOG_BASE = 0x40021800UL;
inline constexpr std::uintptr_t GPIOH_BASE = 0x40021C00UL;
inline constexpr std::uintptr_t GPIOI_BASE = 0x40022000UL;

inline constexpr std::uintptr_t RCC_BASE = 0x40023800UL;

// ---------------------------------------------------------------------------
//  GPIO register block (offsets per RM0090 section 8.4)
// ---------------------------------------------------------------------------
struct GpioRegisters {
    volatile std::uint32_t MODER;    // 0x00 - mode register             (2 bits/pin)
    volatile std::uint32_t OTYPER;   // 0x04 - output type register      (1 bit/pin)
    volatile std::uint32_t OSPEEDR;  // 0x08 - output speed register     (2 bits/pin)
    volatile std::uint32_t PUPDR;    // 0x0C - pull-up/pull-down register (2 bits/pin)
    volatile std::uint32_t IDR;      // 0x10 - input data register       (read only)
    volatile std::uint32_t ODR;      // 0x14 - output data register
    volatile std::uint32_t BSRR;     // 0x18 - bit set/reset register    (write only)
    volatile std::uint32_t LCKR;     // 0x1C - configuration lock register
    volatile std::uint32_t AFRL;     // 0x20 - alternate function low    (pins 0-7)
    volatile std::uint32_t AFRH;     // 0x24 - alternate function high   (pins 8-15)
};

// ---------------------------------------------------------------------------
//  RCC register block (subset: AHB1 clock enable)
// ---------------------------------------------------------------------------
struct RccRegisters {
    volatile std::uint32_t CR;          // 0x00
    volatile std::uint32_t PLLCFGR;     // 0x04
    volatile std::uint32_t CFGR;        // 0x08
    volatile std::uint32_t CIR;         // 0x0C
    volatile std::uint32_t AHB1RSTR;    // 0x10
    volatile std::uint32_t AHB2RSTR;    // 0x14
    volatile std::uint32_t AHB3RSTR;    // 0x18
    volatile std::uint32_t RESERVED0;   // 0x1C
    volatile std::uint32_t APB1RSTR;    // 0x20
    volatile std::uint32_t APB2RSTR;    // 0x24
    volatile std::uint32_t RESERVED1[2];// 0x28-0x2F
    volatile std::uint32_t AHB1ENR;     // 0x30 - AHB1 peripheral clock enable
    volatile std::uint32_t AHB2ENR;     // 0x34
    volatile std::uint32_t AHB3ENR;     // 0x38
    volatile std::uint32_t RESERVED2;   // 0x3C
    volatile std::uint32_t APB1ENR;     // 0x40
    volatile std::uint32_t APB2ENR;     // 0x44
};

// RCC_AHB1ENR GPIO clock enable bits (one per port A..I)
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOAEN = (1U << 0);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOBEN = (1U << 1);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOCEN = (1U << 2);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIODEN = (1U << 3);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOEEN = (1U << 4);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOFEN = (1U << 5);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOGEN = (1U << 6);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOHEN = (1U << 7);
inline constexpr std::uint32_t RCC_AHB1ENR_GPIOIEN = (1U << 8);

// Convenience accessor for the RCC block (hardware only).
inline RccRegisters* rcc() {
    return reinterpret_cast<RccRegisters*>(RCC_BASE);
}

}  // namespace stm32f407

#endif  // STM32F407_REGISTERS_HPP
