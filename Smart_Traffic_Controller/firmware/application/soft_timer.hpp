/**
 * @file    soft_timer.hpp
 * @brief   Wraparound-safe software one-shot timer built on an IClock.
 *
 * Implements hal::ITimer purely in terms of a millisecond clock. The expiry
 * test uses unsigned-difference arithmetic:
 *
 *     elapsed = (uint32_t)(now - start);
 *     expired = elapsed >= duration;
 *
 * which behaves correctly across the 2^32 ms counter wrap because modular
 * subtraction yields the true elapsed interval as long as the interval is
 * shorter than 2^31 ms (~24.8 days) — far longer than any traffic phase.
 *
 * Platform-independent: lives in the application layer and is the same code on
 * target and host. No allocation, no exceptions.
 */
#ifndef APP_SOFT_TIMER_HPP
#define APP_SOFT_TIMER_HPP

#include "hal/i_clock.hpp"
#include "hal/i_timer.hpp"

#include <cstdint>

namespace app {

/// One-shot countdown timer driven by a hal::IClock.
class SoftTimer final : public hal::ITimer {
public:
    explicit SoftTimer(const hal::IClock& clock) noexcept : clock_(clock) {}

    void startMs(std::uint32_t duration_ms) noexcept override {
        start_    = clock_.nowMs();
        duration_ = duration_ms;
        running_  = true;
    }

    void stop() noexcept override { running_ = false; }

    bool expired() const noexcept override {
        if (!running_) {
            return false;
        }
        const std::uint32_t elapsed = clock_.nowMs() - start_; // wraparound-safe
        return elapsed >= duration_;
    }

    bool isRunning() const noexcept override { return running_; }

    std::uint32_t remainingMs() const noexcept override {
        if (!running_) {
            return 0;
        }
        const std::uint32_t elapsed = clock_.nowMs() - start_;
        return (elapsed >= duration_) ? 0U : (duration_ - elapsed);
    }

private:
    const hal::IClock& clock_;
    std::uint32_t      start_    = 0;
    std::uint32_t      duration_ = 0;
    bool               running_  = false;
};

} // namespace app

#endif // APP_SOFT_TIMER_HPP
