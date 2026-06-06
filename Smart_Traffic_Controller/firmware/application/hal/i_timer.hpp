/**
 * @file    i_timer.hpp
 * @brief   Abstract non-blocking one-shot countdown timer (hardware abstraction).
 *
 * A timer is armed with a duration and then polled with expired(); it never
 * blocks. This is the only timing primitive the traffic-light state machine
 * uses, which keeps the state machine free of any clock or delay details and
 * trivially unit-testable.
 *
 * The concrete SoftTimer (soft_timer.hpp) implements this on top of an IClock
 * using wraparound-safe arithmetic. The interface is intentionally clock-free
 * so a hardware timer peripheral could implement it instead.
 */
#ifndef HAL_I_TIMER_HPP
#define HAL_I_TIMER_HPP

#include <cstdint>

namespace hal {

/// Abstract non-blocking one-shot countdown timer.
class ITimer {
public:
    virtual ~ITimer() = default;

    /// Arm the timer to expire @p duration_ms from now and start it running.
    virtual void startMs(std::uint32_t duration_ms) noexcept = 0;

    /// Stop the timer; expired() then reports false until re-armed.
    virtual void stop() noexcept = 0;

    /// True only when the timer is running and its duration has elapsed.
    virtual bool expired() const noexcept = 0;

    /// True while the timer is armed (independent of whether it has expired).
    virtual bool isRunning() const noexcept = 0;

    /// Milliseconds remaining until expiry (0 once expired or stopped).
    virtual std::uint32_t remainingMs() const noexcept = 0;
};

} // namespace hal

#endif // HAL_I_TIMER_HPP
