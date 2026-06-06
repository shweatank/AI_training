/**
 * @file    test_ring_buffer.cpp
 * @brief   Host unit tests for the SPSC ring buffer.
 */
#include "ring_buffer.hpp"
#include "test_framework.hpp"

using app::RingBuffer;

TEST_CASE(ring_starts_empty) {
    RingBuffer<std::uint8_t, 4> rb;
    EXPECT_TRUE(rb.empty());
    EXPECT_FALSE(rb.full());
    EXPECT_EQ(rb.size(), 0U);
    std::uint8_t out = 0xFF;
    EXPECT_FALSE(rb.pop(out)); // nothing to pop
}

TEST_CASE(ring_push_pop_fifo_order) {
    RingBuffer<std::uint8_t, 8> rb;
    EXPECT_TRUE(rb.push(1));
    EXPECT_TRUE(rb.push(2));
    EXPECT_TRUE(rb.push(3));
    EXPECT_EQ(rb.size(), 3U);

    std::uint8_t v = 0;
    EXPECT_TRUE(rb.pop(v)); EXPECT_EQ(v, 1U);
    EXPECT_TRUE(rb.pop(v)); EXPECT_EQ(v, 2U);
    EXPECT_TRUE(rb.pop(v)); EXPECT_EQ(v, 3U);
    EXPECT_TRUE(rb.empty());
}

TEST_CASE(ring_full_capacity_is_one_less_than_slots) {
    RingBuffer<std::uint8_t, 4> rb; // usable capacity = 3
    const std::size_t max_items = RingBuffer<std::uint8_t, 4>::kMaxItems;
    EXPECT_EQ(max_items, 3U);
    EXPECT_TRUE(rb.push(10));
    EXPECT_TRUE(rb.push(20));
    EXPECT_TRUE(rb.push(30));
    EXPECT_TRUE(rb.full());
    EXPECT_FALSE(rb.push(40)); // dropped, buffer full
    EXPECT_EQ(rb.size(), 3U);
}

TEST_CASE(ring_wraps_around_indices) {
    RingBuffer<std::uint8_t, 4> rb; // 3 usable
    std::uint8_t v = 0;
    // Fill, drain, refill to force head/tail past the end.
    for (std::uint8_t i = 0; i < 3; ++i) EXPECT_TRUE(rb.push(i));
    for (std::uint8_t i = 0; i < 3; ++i) { EXPECT_TRUE(rb.pop(v)); EXPECT_EQ(v, i); }
    EXPECT_TRUE(rb.empty());
    for (std::uint8_t i = 100; i < 103; ++i) EXPECT_TRUE(rb.push(i));
    for (std::uint8_t i = 100; i < 103; ++i) { EXPECT_TRUE(rb.pop(v)); EXPECT_EQ(v, i); }
    EXPECT_TRUE(rb.empty());
}

TEST_CASE(ring_clear_discards_contents) {
    RingBuffer<std::uint8_t, 8> rb;
    rb.push(1); rb.push(2);
    rb.clear();
    EXPECT_TRUE(rb.empty());
    std::uint8_t out = 0;
    EXPECT_FALSE(rb.pop(out));
}
