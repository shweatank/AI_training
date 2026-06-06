/**
 * @file    test_traffic_light.cpp
 * @brief   Host unit tests for the traffic-light state machine.
 *
 * Uses the real SoftTimer driven by a FakeClock and FakeLed lamps, so the test
 * exercises the genuine timing path without any hardware.
 */
#include "traffic_light.hpp"
#include "test_framework.hpp"

#include "mocks.hpp"
#include "soft_timer.hpp"

using app::SoftTimer;
using app::TrafficLight;
using app::TrafficState;
using mock::FakeClock;
using mock::FakeLed;

namespace {

/// Bundles a light with its dependencies so each test gets a fresh rig.
struct Rig {
    FakeClock  clock;
    FakeLed    red, yellow, green, status;
    SoftTimer  timer{clock};
    TrafficLight light{red, yellow, green, status, timer};
};

void expectOnlyLit(Rig& r, const FakeLed& lit) {
    EXPECT_EQ(r.red.isOn(),    (&r.red == &lit));
    EXPECT_EQ(r.yellow.isOn(), (&r.yellow == &lit));
    EXPECT_EQ(r.green.isOn(),  (&r.green == &lit));
}

} // namespace

TEST_CASE(traffic_initial_state_is_idle_all_dark) {
    Rig r;
    EXPECT_TRUE(r.light.state() == TrafficState::Idle);
    EXPECT_FALSE(r.light.isRunning());
    EXPECT_FALSE(r.red.isOn());
    EXPECT_FALSE(r.yellow.isOn());
    EXPECT_FALSE(r.green.isOn());
    EXPECT_FALSE(r.status.isOn());
}

TEST_CASE(traffic_start_enters_red_and_lights_status) {
    Rig r;
    r.light.start();
    EXPECT_TRUE(r.light.state() == TrafficState::Red);
    EXPECT_TRUE(r.light.isRunning());
    expectOnlyLit(r, r.red);
    EXPECT_TRUE(r.status.isOn()); // RUN indicator on
}

TEST_CASE(traffic_cycles_red_green_yellow_red) {
    Rig r;
    r.light.start(); // RED for 30 s by default

    // Not yet elapsed -> no transition.
    r.clock.advance(30000 - 1);
    EXPECT_FALSE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Red);

    // RED -> GREEN at 30 s.
    r.clock.advance(1);
    EXPECT_TRUE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Green);
    expectOnlyLit(r, r.green);

    // GREEN -> YELLOW at +20 s.
    r.clock.advance(20000);
    EXPECT_TRUE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Yellow);
    expectOnlyLit(r, r.yellow);

    // YELLOW -> RED at +5 s.
    r.clock.advance(5000);
    EXPECT_TRUE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Red);
    expectOnlyLit(r, r.red);
}

TEST_CASE(traffic_stop_returns_to_idle_all_dark) {
    Rig r;
    r.light.start();
    r.light.stop();
    EXPECT_TRUE(r.light.state() == TrafficState::Idle);
    EXPECT_FALSE(r.light.isRunning());
    EXPECT_FALSE(r.red.isOn());
    EXPECT_FALSE(r.yellow.isOn());
    EXPECT_FALSE(r.green.isOn());
    EXPECT_FALSE(r.status.isOn());
}

TEST_CASE(traffic_update_does_nothing_when_idle) {
    Rig r;
    r.clock.advance(1000000);
    EXPECT_FALSE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Idle);
}

TEST_CASE(traffic_set_durations_take_effect_next_cycle) {
    Rig r;
    r.light.setRedDuration(10);
    r.light.setGreenDuration(8);
    r.light.setYellowDuration(2);
    EXPECT_EQ(r.light.config().red_sec, 10U);
    EXPECT_EQ(r.light.config().green_sec, 8U);
    EXPECT_EQ(r.light.config().yellow_sec, 2U);

    r.light.start();
    r.clock.advance(10000);
    EXPECT_TRUE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Green); // used new 10 s red
}

TEST_CASE(traffic_set_active_phase_rearms_timer) {
    Rig r;
    r.light.start(); // RED, 30 s
    r.clock.advance(29000);
    // Extend the active RED phase to 60 s; timer re-arms from "now".
    r.light.setRedDuration(60);
    r.clock.advance(30000); // 30 s into the *re-armed* 60 s phase
    EXPECT_FALSE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Red);
    r.clock.advance(30000); // now 60 s since re-arm
    EXPECT_TRUE(r.light.update());
    EXPECT_TRUE(r.light.state() == TrafficState::Green);
}

TEST_CASE(traffic_remaining_seconds_rounds_up) {
    Rig r;
    r.light.start(); // RED 30 s
    EXPECT_EQ(r.light.remainingSec(), 30U);
    r.clock.advance(500); // 29.5 s remain -> rounds up to 30
    EXPECT_EQ(r.light.remainingSec(), 30U);
    r.clock.advance(28500); // 1.0 s remain
    EXPECT_EQ(r.light.remainingSec(), 1U);
    r.clock.advance(1000); // expired
    EXPECT_EQ(r.light.remainingSec(), 0U);
}
