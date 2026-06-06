/**
 * @file    stm32f407_regs.hpp
 * @brief   Memory-mapped register definitions for the STM32F407 (Cortex-M4).
 *
 * Register blocks are modelled as POD structs of `volatile` 32-bit words and
 * accessed through pointers, so the same driver code can run against real MMIO
 * on the target or against ordinary host RAM in unit tests.
 *
 * Reference: RM0090 "STM32F405/415, STM32F407/417 reference manual" and the
 * ARMv7-M architecture reference (SysTick / NVIC in the System Control Space).
 *
 * C++17, freestanding: no dynamic allocation, no exceptions, no RTTI.
 */
#ifndef STM32F407_REGS_HPP
#define STM32F407_REGS_HPP

#include <cstdint>

namespace stm32f407 {

/// 32-bit hardware register (read/write).
using reg32_t = volatile std::uint32_t;

// ---------------------------------------------------------------------------
// GPIO (RM0090 §8.4)
// ---------------------------------------------------------------------------
struct GpioRegs {
    reg32_t MODER;   ///< 0x00 mode register
    reg32_t OTYPER;  ///< 0x04 output type register
    reg32_t OSPEEDR; ///< 0x08 output speed register
    reg32_t PUPDR;   ///< 0x0C pull-up/pull-down register
    reg32_t IDR;     ///< 0x10 input data register
    reg32_t ODR;     ///< 0x14 output data register
    reg32_t BSRR;    ///< 0x18 bit set/reset register
    reg32_t LCKR;    ///< 0x1C configuration lock register
    reg32_t AFR[2];  ///< 0x20-0x24 alternate function low/high
};

// ---------------------------------------------------------------------------
// RCC (RM0090 §7.3)
// ---------------------------------------------------------------------------
struct RccRegs {
    reg32_t CR;         ///< 0x00 clock control
    reg32_t PLLCFGR;    ///< 0x04 PLL configuration
    reg32_t CFGR;       ///< 0x08 clock configuration
    reg32_t CIR;        ///< 0x0C clock interrupt
    reg32_t AHB1RSTR;   ///< 0x10
    reg32_t AHB2RSTR;   ///< 0x14
    reg32_t AHB3RSTR;   ///< 0x18
    reg32_t _reserved0; ///< 0x1C
    reg32_t APB1RSTR;   ///< 0x20
    reg32_t APB2RSTR;   ///< 0x24
    reg32_t _reserved1[2];
    reg32_t AHB1ENR;    ///< 0x30 AHB1 peripheral clock enable
    reg32_t AHB2ENR;    ///< 0x34
    reg32_t AHB3ENR;    ///< 0x38
    reg32_t _reserved2; ///< 0x3C
    reg32_t APB1ENR;    ///< 0x40 APB1 peripheral clock enable
    reg32_t APB2ENR;    ///< 0x44 APB2 peripheral clock enable
};

// ---------------------------------------------------------------------------
// USART (RM0090 §30.6)
// ---------------------------------------------------------------------------
struct UsartRegs {
    reg32_t SR;   ///< 0x00 status register
    reg32_t DR;   ///< 0x04 data register
    reg32_t BRR;  ///< 0x08 baud rate register
    reg32_t CR1;  ///< 0x0C control register 1
    reg32_t CR2;  ///< 0x10 control register 2
    reg32_t CR3;  ///< 0x14 control register 3
    reg32_t GTPR; ///< 0x18 guard time and prescaler
};

// ---------------------------------------------------------------------------
// SysTick system timer (ARMv7-M, SCS @ 0xE000E010)
// ---------------------------------------------------------------------------
struct SysTickRegs {
    reg32_t CTRL;  ///< 0x00 control and status
    reg32_t LOAD;  ///< 0x04 reload value
    reg32_t VAL;   ///< 0x08 current value
    reg32_t CALIB; ///< 0x0C calibration
};

// ---------------------------------------------------------------------------
// NVIC interrupt set-enable (ARMv7-M, SCS @ 0xE000E100)
// ---------------------------------------------------------------------------
struct NvicRegs {
    reg32_t ISER[8]; ///< 0x000-0x01C set-enable
    reg32_t _gap0[24];
    reg32_t ICER[8]; ///< 0x080-0x09C clear-enable
};

// ---------------------------------------------------------------------------
// Peripheral base addresses (RM0090 §2.3 memory map)
// ---------------------------------------------------------------------------
namespace addr {
constexpr std::uintptr_t kPeriphBase = 0x40000000UL;
constexpr std::uintptr_t kApb1Base   = kPeriphBase;
constexpr std::uintptr_t kApb2Base   = kPeriphBase + 0x00010000UL;
constexpr std::uintptr_t kAhb1Base   = kPeriphBase + 0x00020000UL;

constexpr std::uintptr_t kGpioA = kAhb1Base + 0x0000UL;
constexpr std::uintptr_t kGpioB = kAhb1Base + 0x0400UL;
constexpr std::uintptr_t kGpioC = kAhb1Base + 0x0800UL;
constexpr std::uintptr_t kGpioD = kAhb1Base + 0x0C00UL;
constexpr std::uintptr_t kGpioE = kAhb1Base + 0x1000UL;
constexpr std::uintptr_t kRcc   = kAhb1Base + 0x3800UL;

constexpr std::uintptr_t kUsart1 = kApb2Base + 0x1000UL;
constexpr std::uintptr_t kUsart2 = kApb1Base + 0x4400UL;
constexpr std::uintptr_t kUsart3 = kApb1Base + 0x4800UL;

constexpr std::uintptr_t kSysTick = 0xE000E010UL;
constexpr std::uintptr_t kNvic    = 0xE000E100UL;
} // namespace addr

// ---------------------------------------------------------------------------
// Register-pointer accessors. On target these resolve to fixed MMIO addresses;
// host tests never link them and supply their own structs instead.
// ---------------------------------------------------------------------------
template <typename T>
inline T* periph(std::uintptr_t address) {
    return reinterpret_cast<T*>(address);
}

inline GpioRegs*    GPIOA()   { return periph<GpioRegs>(addr::kGpioA); }
inline GpioRegs*    GPIOB()   { return periph<GpioRegs>(addr::kGpioB); }
inline GpioRegs*    GPIOC()   { return periph<GpioRegs>(addr::kGpioC); }
inline GpioRegs*    GPIOD()   { return periph<GpioRegs>(addr::kGpioD); }
inline GpioRegs*    GPIOE()   { return periph<GpioRegs>(addr::kGpioE); }
inline RccRegs*     RCC()     { return periph<RccRegs>(addr::kRcc); }
inline UsartRegs*   USART1()  { return periph<UsartRegs>(addr::kUsart1); }
inline UsartRegs*   USART2()  { return periph<UsartRegs>(addr::kUsart2); }
inline UsartRegs*   USART3()  { return periph<UsartRegs>(addr::kUsart3); }
inline SysTickRegs* SYSTICK() { return periph<SysTickRegs>(addr::kSysTick); }
inline NvicRegs*    NVIC()    { return periph<NvicRegs>(addr::kNvic); }

// ---------------------------------------------------------------------------
// IRQ numbers used by the firmware (RM0090 Table 62 vector table).
// ---------------------------------------------------------------------------
namespace irq {
constexpr std::uint32_t kUsart2 = 38; ///< USART2 global interrupt
} // namespace irq

// ---------------------------------------------------------------------------
// RCC clock-enable bit positions.
// ---------------------------------------------------------------------------
namespace rcc_bits {
// AHB1ENR
constexpr std::uint32_t kGpioAEn = (1U << 0);
constexpr std::uint32_t kGpioBEn = (1U << 1);
constexpr std::uint32_t kGpioCEn = (1U << 2);
constexpr std::uint32_t kGpioDEn = (1U << 3);
constexpr std::uint32_t kGpioEEn = (1U << 4);
// APB1ENR
constexpr std::uint32_t kUsart2En = (1U << 17);
constexpr std::uint32_t kUsart3En = (1U << 18);
// APB2ENR
constexpr std::uint32_t kUsart1En = (1U << 4);
} // namespace rcc_bits

// ---------------------------------------------------------------------------
// USART bit fields.
// ---------------------------------------------------------------------------
namespace usart_bits {
// SR
constexpr std::uint32_t kRXNE = (1U << 5); ///< read data register not empty
constexpr std::uint32_t kTC   = (1U << 6); ///< transmission complete
constexpr std::uint32_t kTXE  = (1U << 7); ///< transmit data register empty
// CR1
constexpr std::uint32_t kRE     = (1U << 2);  ///< receiver enable
constexpr std::uint32_t kTE     = (1U << 3);  ///< transmitter enable
constexpr std::uint32_t kRXNEIE = (1U << 5);  ///< RXNE interrupt enable
constexpr std::uint32_t kPS     = (1U << 9);  ///< parity selection (0=even,1=odd)
constexpr std::uint32_t kPCE    = (1U << 10); ///< parity control enable
constexpr std::uint32_t kM      = (1U << 12); ///< word length (0=8,1=9)
constexpr std::uint32_t kUE     = (1U << 13); ///< USART enable
// CR2
constexpr std::uint32_t kStopShift = 12;      ///< STOP[1:0] position
constexpr std::uint32_t kStopMask  = (0x3U << kStopShift);
} // namespace usart_bits

// ---------------------------------------------------------------------------
// SysTick CTRL bit fields (ARMv7-M B3.3).
// ---------------------------------------------------------------------------
namespace systick_bits {
constexpr std::uint32_t kEnable    = (1U << 0); ///< counter enable
constexpr std::uint32_t kTickInt   = (1U << 1); ///< SysTick exception request enable
constexpr std::uint32_t kClkSource = (1U << 2); ///< 1 = processor clock
constexpr std::uint32_t kReloadMax = 0x00FFFFFFUL; ///< 24-bit reload field
} // namespace systick_bits

} // namespace stm32f407

#endif // STM32F407_REGS_HPP
