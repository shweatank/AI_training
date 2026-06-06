/**
 * @file    traffic_light.hpp
 * @brief   Non-blocking, timer-driven traffic-light state machine.
 *
 * Owns three signal lamps (red / yellow / green) and a status lamp, plus a
 * one-shot ITimer. When running it cycles RED -> GREEN -> YELLOW -> RED, each
 * phase lasting its configured duration. Advancement happens only inside
 * update(), which the caller invokes every main-loop iteration; there are no
 * blocking delays anywhere.
 *
 * The machine is entirely platform-independent — it talks only to hal::ILed
 * and hal::ITimer — so it runs unchanged on the target and under host tests
 * with fakes. No allocation, no exceptions, no recursion.
 */
#ifndef APP_TRAFFIC_LIGHT_HPP
#define APP_TRAFFIC_LIGHT_HPP

#include "hal/i_led.hpp"
#include "hal/i_timer.hpp"

#include <cstdint>

namespace app {

/// Signal phases. IDLE means stopped (no lamp lit except the status lamp off).
enum class TrafficState : std::uint8_t {
    Idle,
    Red,
    Green,
    Yellow,
};

/// Configurable phase durations, in seconds.
struct TrafficConfig {
    std::uint32_t red_sec    = 30;
    std::uint32_t green_sec  = 20;
    std::uint32_t yellow_sec = 5;
};

/**
 * @brief Timer-driven traffic-light controller.
 *
 * Lamps and timer are injected by reference and must outlive the instance.
 */
class TrafficLight {
public:
    TrafficLight(hal::ILed& red, hal::ILed& yellow, hal::ILed& green,
                 hal::ILed& status, hal::ITimer& timer) noexcept;

    /// Begin (or restart) cycling from the RED phase. Idempotent-safe to call
    /// while already running (it simply restarts at RED).
    void start() noexcept;

    /// Halt cycling, extinguish all signal lamps, and enter IDLE.
    void stop() noexcept;

    /// Advance the state machine if the current phase has elapsed. Call once
    /// per main-loop iteration. Returns true if a phase transition occurred.
    bool update() noexcept;

    /// Set the red-phase duration (seconds). Takes effect on the next RED entry;
    /// if RED is active and running, the running phase is re-armed.
    void setRedDuration(std::uint32_t seconds) noexcept;
    void setGreenDuration(std::uint32_t seconds) noexcept;
    void setYellowDuration(std::uint32_t seconds) noexcept;

    TrafficState         state() const noexcept { return state_; }
    bool                 isRunning() const noexcept { return running_; }
    const TrafficConfig& config() const noexcept { return config_; }

    /// Seconds remaining in the current phase (0 when idle/stopped).
    std::uint32_t remainingSec() const noexcept;

private:
    /// Drive the lamps to match @p state and arm the timer for its duration.
    void enterState(TrafficState state) noexcept;

    /// Duration (seconds) configured for @p state; 0 for IDLE.
    std::uint32_t durationFor(TrafficState state) const noexcept;

    /// Re-arm the active timer if @p changed matches the running phase.
    void reArmIfActive(TrafficState changed) noexcept;

    hal::ILed&    red_;
    hal::ILed&    yellow_;
    hal::ILed&    green_;
    hal::ILed&    status_;
    hal::ITimer&  timer_;
    TrafficConfig config_{};
    TrafficState  state_   = TrafficState::Idle;
    bool          running_ = false;
};

} // namespace app

#endif // APP_TRAFFIC_LIGHT_HPP
