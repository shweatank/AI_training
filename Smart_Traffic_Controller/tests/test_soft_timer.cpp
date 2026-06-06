/**
 * @file    test_soft_timer.cpp
 * @brief   Host unit tests for the wraparound-safe software timer.
 */
#include "soft_timer.hpp"
#include "test_framework.hpp"

#include "mocks.hpp"

using app::SoftTimer;
using mock::FakeClock;

TEST_CASE(timer_not_running_until_started) {
    FakeClock clk;
    SoftTimer t(clk);
    EXPECT_FALSE(t.isRunning());
    EXPECT_FALSE(t.expired());
    EXPECT_EQ(t.remainingMs(), 0U);
}

TEST_CASE(timer_expires_exactly_at_duration) {
    FakeClock clk;
    SoftTimer t(clk);
    t.startMs(1000);
    EXPECT_TRUE(t.isRunning());

    clk.advance(999);
    EXPECT_FALSE(t.expired());
    EXPECT_EQ(t.remainingMs(), 1U);

    clk.advance(1); // now exactly 1000 ms elapsed
    EXPECT_TRUE(t.expired());
    EXPECT_EQ(t.remainingMs(), 0U);
}

TEST_CASE(timer_stop_clears_running_and_expiry) {
    FakeClock clk;
    SoftTimer t(clk);
    t.startMs(500);
    clk.advance(600);
    EXPECT_TRUE(t.expired());
    t.stop();
    EXPECT_FALSE(t.isRunning());
    EXPECT_FALSE(t.expired());
}

TEST_CASE(timer_handles_counter_wraparound) {
    FakeClock clk;
    SoftTimer t(clk);
    // Start near the 32-bit wrap point.
    clk.setNow(0xFFFFFF00U);
    t.startMs(0x200); // 512 ms; expiry time wraps past 0
    EXPECT_FALSE(t.expired());

    // Advance 0x100 ms -> still 0x100 short, no wrap reached yet.
    clk.setNow(0xFFFFFF00U + 0x100U);
    EXPECT_FALSE(t.expired());
    EXPECT_EQ(t.remainingMs(), 0x100U);

    // Advance past the wrap: now = 0x100 (i.e. 0xFFFFFF00 + 0x200 mod 2^32).
    clk.setNow(0x100U);
    EXPECT_TRUE(t.expired()); // unsigned-difference arithmetic stays correct
}
