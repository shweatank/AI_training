/**
 * @file    test_command_processor.cpp
 * @brief   Host unit tests for the end-to-end command processor.
 *
 * Wires a real TrafficLight (SoftTimer + FakeClock + FakeLeds) to a FakeUart so
 * we can assert on the exact ACK/NACK/STATUS bytes emitted for each command.
 */
#include "command_processor.hpp"
#include "test_framework.hpp"

#include "mocks.hpp"
#include "soft_timer.hpp"
#include "traffic_light.hpp"

#include <cstring>

using app::CommandProcessor;
using app::SoftTimer;
using app::TrafficLight;
using app::TrafficState;
using mock::FakeClock;
using mock::FakeLed;
using mock::FakeUart;

namespace {

struct Rig {
    FakeClock    clock;
    FakeLed      red, yellow, green, status;
    SoftTimer    timer{clock};
    TrafficLight light{red, yellow, green, status, timer};
    FakeUart     uart;
    CommandProcessor proc{light, uart};

    void send(const char* line) {
        uart.clearTx();
        proc.handleLine(line, std::strlen(line));
    }
};

} // namespace

TEST_CASE(proc_valid_command_acks) {
    Rig r;
    r.send("R30");
    EXPECT_TRUE(r.uart.txContains("ACK\r\n"));
    EXPECT_FALSE(r.uart.txContains("NACK"));
    EXPECT_EQ(r.light.config().red_sec, 30U);
}

TEST_CASE(proc_invalid_command_nacks) {
    Rig r;
    r.send("BOGUS");
    // Invalid input yields exactly NACK and nothing else (no STATUS echo).
    EXPECT_TRUE(r.uart.tx() == "NACK\r\n");
}

TEST_CASE(proc_set_command_echoes_status) {
    Rig r;
    r.send("G20");
    EXPECT_TRUE(r.uart.txContains("ACK\r\n"));
    EXPECT_TRUE(r.uart.txContains("STATUS STATE=IDLE"));
    EXPECT_TRUE(r.uart.txContains("G=20"));
}

TEST_CASE(proc_start_runs_and_reports_red) {
    Rig r;
    r.send("START");
    EXPECT_TRUE(r.uart.txContains("ACK\r\n"));
    EXPECT_TRUE(r.uart.txContains("STATUS STATE=RED RUN=1"));
    EXPECT_TRUE(r.light.isRunning());
    EXPECT_TRUE(r.light.state() == TrafficState::Red);
}

TEST_CASE(proc_stop_returns_idle) {
    Rig r;
    r.send("START");
    r.send("STOP");
    EXPECT_TRUE(r.uart.txContains("ACK\r\n"));
    EXPECT_TRUE(r.uart.txContains("STATUS STATE=IDLE RUN=0"));
    EXPECT_FALSE(r.light.isRunning());
}

TEST_CASE(proc_status_reports_all_timings) {
    Rig r;
    r.send("R30");
    r.send("G20");
    r.send("Y5");
    r.send("STATUS");
    EXPECT_TRUE(r.uart.txContains("ACK\r\n"));
    EXPECT_TRUE(r.uart.txContains("R=30"));
    EXPECT_TRUE(r.uart.txContains("G=20"));
    EXPECT_TRUE(r.uart.txContains("Y=5"));
}

TEST_CASE(proc_help_lists_commands) {
    Rig r;
    r.send("HELP");
    EXPECT_TRUE(r.uart.txContains("ACK\r\n"));
    EXPECT_TRUE(r.uart.txContains("Commands:"));
    EXPECT_TRUE(r.uart.txContains("START"));
    EXPECT_TRUE(r.uart.txContains("HELP"));
}

TEST_CASE(proc_case_insensitive_lowercase_start) {
    Rig r;
    r.send("start");
    EXPECT_TRUE(r.uart.txContains("ACK\r\n"));
    EXPECT_TRUE(r.light.isRunning());
}

TEST_CASE(proc_out_of_range_duration_nacks_without_changing_state) {
    Rig r;
    const std::uint32_t before = r.light.config().red_sec;
    r.send("R0");      // below minimum
    EXPECT_TRUE(r.uart.txContains("NACK\r\n"));
    EXPECT_EQ(r.light.config().red_sec, before); // unchanged
    r.send("R5000");   // above maximum
    EXPECT_TRUE(r.uart.txContains("NACK\r\n"));
    EXPECT_EQ(r.light.config().red_sec, before);
}

TEST_CASE(proc_status_shows_remaining_after_advance) {
    Rig r;
    r.send("R10");
    r.send("START"); // RED 10 s
    r.clock.advance(3000);
    r.send("STATUS");
    // 7 s remain -> "REMAIN=7" appears in the status line.
    EXPECT_TRUE(r.uart.txContains("REMAIN=7"));
}
