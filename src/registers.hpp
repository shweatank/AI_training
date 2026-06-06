// registers.hpp — minimal memory-mapped peripheral definitions for STM32F407.
//
// Bare-metal, no vendor HAL. Only the registers needed for GPIO blinking are
// declared. Everything is constexpr / volatile-correct so the compiler emits
// direct load/store instructions with zero runtime overhead.
#pragma once

#include <cstdint>

namespace stm32 {

using reg32_t = volatile std::uint32_t;

// ---- Memory map base addresses (RM0090, Table 1) -------------------------
inline constexpr std::uintptr_t kPeriphBase = 0x40000000UL;
inline constexpr std::uintptr_t kAhb1Base   = kPeriphBase + 0x00020000UL; // 0x40020000
inline constexpr std::uintptr_t kRccBase    = kAhb1Base   + 0x00003800UL; // 0x40023800
inline constexpr std::uintptr_t kGpiodBase  = kAhb1Base   + 0x00000C00UL; // 0x40020C00

// ---- Reset and Clock Control (RCC) ---------------------------------------
struct Rcc {
    reg32_t CR;          // 0x00
    reg32_t PLLCFGR;     // 0x04
    reg32_t CFGR;        // 0x08
    reg32_t CIR;         // 0x0C
    reg32_t AHB1RSTR;    // 0x10
    reg32_t AHB2RSTR;    // 0x14
    reg32_t AHB3RSTR;    // 0x18
    reg32_t _reserved0;  // 0x1C
    reg32_t APB1RSTR;    // 0x20
    reg32_t APB2RSTR;    // 0x24
    reg32_t _reserved1[2];
    reg32_t AHB1ENR;     // 0x30  <- GPIO port clock enables live here
};

// ---- General-purpose I/O (GPIO) ------------------------------------------
struct Gpio {
    reg32_t MODER;    // 0x00  port mode
    reg32_t OTYPER;   // 0x04  output type
    reg32_t OSPEEDR;  // 0x08  output speed
    reg32_t PUPDR;    // 0x0C  pull-up/pull-down
    reg32_t IDR;      // 0x10  input data
    reg32_t ODR;      // 0x14  output data
    reg32_t BSRR;     // 0x18  bit set/reset (atomic)
    reg32_t LCKR;     // 0x1C  configuration lock
    reg32_t AFR[2];   // 0x20  alternate function low/high
};

// Typed accessors — `reinterpret_cast` of a fixed address is the idiomatic
// way to reach a memory-mapped peripheral from C++.
inline Rcc&  rcc()   { return *reinterpret_cast<Rcc*>(kRccBase); }
inline Gpio& gpiod() { return *reinterpret_cast<Gpio*>(kGpiodBase); }

// RCC_AHB1ENR bit for GPIOD clock (RM0090, 6.3.10).
inline constexpr std::uint32_t kRccAhb1En_GpioD = (1U << 3);

} // namespace stm32
