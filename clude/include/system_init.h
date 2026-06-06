/**
 * @file system_init.h
 * @brief System Initialization for STM32F407
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include <cstdint>

namespace system {

class SystemInit {
public:
    /**
     * @brief Initialize system clocks
     * @return true if successful
     */
    static bool initClocks();

    /**
     * @brief Initialize all peripherals
     * @return true if successful
     */
    static bool initPeripherals();

    /**
     * @brief Initialize SysTick timer
     * @return true if successful
     */
    static bool initSysTick();

    /**
     * @brief Get system tick counter (milliseconds)
     * @return Tick count in milliseconds
     */
    static uint32_t getSysTick();

    /**
     * @brief Reset system tick counter
     */
    static void resetSysTick();

    /**
     * @brief Get system clock frequency (Hz)
     * @return Frequency in Hz
     */
    static uint32_t getSystemClockFreq();

    /**
     * @brief Get APB1 clock frequency (Hz)
     * @return Frequency in Hz
     */
    static uint32_t getAPB1ClockFreq();

    /**
     * @brief Get APB2 clock frequency (Hz)
     * @return Frequency in Hz
     */
    static uint32_t getAPB2ClockFreq();

    /**
     * @brief Enable all necessary clocks
     * @return true if successful
     */
    static bool enableAllClocks();

private:
    static constexpr uint32_t SYSTEM_CLOCK_FREQ = 168000000U;  // 168 MHz
    static constexpr uint32_t APB1_CLOCK_FREQ = 42000000U;     // 42 MHz
    static constexpr uint32_t APB2_CLOCK_FREQ = 84000000U;     // 84 MHz
    static constexpr uint32_t SYSTICK_FREQ = 1000U;            // 1 kHz

    static volatile uint32_t systick_counter_;
    static bool initialized_;

    /**
     * @brief Configure PLL for 168 MHz operation
     */
    static void configurePLL();

    /**
     * @brief Configure flash wait states
     */
    static void configureFlash();

    /**
     * @brief Configure prescalers for APB1 and APB2
     */
    static void configurePrescalers();
};

} // namespace system

#endif // SYSTEM_INIT_H
