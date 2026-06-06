/**
 * @file    board.hpp
 * @brief   Board pin / clock map for the STM32F407G-DISC1 (Discovery) board.
 *
 * Traffic signal lamps reuse the four on-board user LEDs on GPIO port D:
 *   PD14 = red    (signal)
 *   PD13 = orange (used as the YELLOW signal)
 *   PD12 = green  (signal)
 *   PD15 = blue   (RUN / status indicator: lit whenever the controller cycles)
 *
 * The serial console is USART2 on PA2 (TX) / PA3 (RX), alternate function AF7.
 * On the Discovery board these are free header pins; connect a 3V3 USB-UART
 * adapter (TX<->RX crossed) at 115200-8N1.
 *
 * Clocking: this firmware runs from the 16 MHz HSI without the PLL, so both the
 * core clock (feeding SysTick) and the APB1 clock (feeding USART2) are 16 MHz.
 */
#ifndef BOARD_HPP
#define BOARD_HPP

#include "stm32f407_gpio.hpp"
#include <cstdint>

namespace board {

// --- Traffic-signal LEDs (GPIOD) -----------------------------------------
constexpr std::uint8_t kLedGreen  = 12; ///< PD12 green signal
constexpr std::uint8_t kLedYellow = 13; ///< PD13 orange, used as yellow signal
constexpr std::uint8_t kLedRed    = 14; ///< PD14 red signal
constexpr std::uint8_t kLedStatus = 15; ///< PD15 blue, RUN indicator

// --- USART2 console pins (GPIOA, AF7) ------------------------------------
constexpr std::uint8_t           kUsart2TxPin = 2;
constexpr std::uint8_t           kUsart2RxPin = 3;
constexpr stm32f407::AltFunction kUsart2Af    = stm32f407::AltFunction::AF7;

// --- Clocks ---------------------------------------------------------------
constexpr std::uint32_t kCoreClock = 16000000; ///< Hz (HSI, no PLL) -> SysTick
constexpr std::uint32_t kApb1Clock = 16000000; ///< Hz (HSI, no PLL) -> USART2

} // namespace board

#endif // BOARD_HPP
