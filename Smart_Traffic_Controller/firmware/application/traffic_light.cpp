/**
 * @file    traffic_light.cpp
 * @brief   Implementation of the traffic-light state machine.
 */
#include "traffic_light.hpp"

namespace app {

namespace {
constexpr std::uint32_t kMsPerSec = 1000U;
} // namespace

TrafficLight::TrafficLight(hal::ILed& red, hal::ILed& yellow, hal::ILed& green,
                           hal::ILed& status, hal::ITimer& timer) noexcept
    : red_(red), yellow_(yellow), green_(green), status_(status), timer_(timer) {
    // Start in a defined, all-dark IDLE state.
    enterState(TrafficState::Idle);
}

std::uint32_t TrafficLight::durationFor(TrafficState state) const noexcept {
    switch (state) {
        case TrafficState::Red:    return config_.red_sec;
        case TrafficState::Green:  return config_.green_sec;
        case TrafficState::Yellow: return config_.yellow_sec;
        case TrafficState::Idle:   default: return 0U;
    }
}

void TrafficLight::enterState(TrafficState state) noexcept {
    state_ = state;

    // Drive signal lamps: exactly one lit per active phase, none when idle.
    red_.set(state == TrafficState::Red);
    yellow_.set(state == TrafficState::Yellow);
    green_.set(state == TrafficState::Green);

    if (state == TrafficState::Idle) {
        status_.off();
        timer_.stop();
        return;
    }

    status_.on(); // status lamp indicates "controller running"
    timer_.startMs(durationFor(state) * kMsPerSec);
}

void TrafficLight::start() noexcept {
    running_ = true;
    enterState(TrafficState::Red); // a cycle always begins on RED
}

void TrafficLight::stop() noexcept {
    running_ = false;
    enterState(TrafficState::Idle);
}

bool TrafficLight::update() noexcept {
    if (!running_ || !timer_.expired()) {
        return false;
    }

    // Fixed RED -> GREEN -> YELLOW -> RED cycle.
    switch (state_) {
        case TrafficState::Red:    enterState(TrafficState::Green);  break;
        case TrafficState::Green:  enterState(TrafficState::Yellow); break;
        case TrafficState::Yellow: enterState(TrafficState::Red);    break;
        case TrafficState::Idle:   default:                          return false;
    }
    return true;
}

void TrafficLight::reArmIfActive(TrafficState changed) noexcept {
    if (running_ && state_ == changed) {
        timer_.startMs(durationFor(changed) * kMsPerSec);
    }
}

void TrafficLight::setRedDuration(std::uint32_t seconds) noexcept {
    config_.red_sec = seconds;
    reArmIfActive(TrafficState::Red);
}

void TrafficLight::setGreenDuration(std::uint32_t seconds) noexcept {
    config_.green_sec = seconds;
    reArmIfActive(TrafficState::Green);
}

void TrafficLight::setYellowDuration(std::uint32_t seconds) noexcept {
    config_.yellow_sec = seconds;
    reArmIfActive(TrafficState::Yellow);
}

std::uint32_t TrafficLight::remainingSec() const noexcept {
    if (!running_ || state_ == TrafficState::Idle) {
        return 0U;
    }
    // Round up to the nearest whole second so a phase reports "1" until it ends.
    return (timer_.remainingMs() + kMsPerSec - 1U) / kMsPerSec;
}

} // namespace app
