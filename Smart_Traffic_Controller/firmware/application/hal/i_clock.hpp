/**
 * @file    i_clock.hpp
 * @brief   Abstract monotonic millisecond time source (hardware abstraction).
 *
 * Provides a free-running millisecond counter. The value is expected to wrap
 * around after 2^32 ms (~49.7 days); all consumers must use unsigned-difference
 * arithmetic so wraparound is handled transparently (see SoftTimer).
 *
 * On the target this is backed by the Cortex-M SysTick (systick_clock.hpp); in
 * tests a fake clock lets cases advance time deterministically.
 */
#ifndef HAL_I_CLOCK_HPP
#define HAL_I_CLOCK_HPP

#include <cstdint>

namespace hal {

/// Abstract monotonic millisecond clock.
class IClock {
public:
    virtual ~IClock() = default;

    /// Milliseconds since boot, free-running and wraparound at 2^32.
    virtual std::uint32_t nowMs() const noexcept = 0;
};

} // namespace hal

#endif // HAL_I_CLOCK_HPP
