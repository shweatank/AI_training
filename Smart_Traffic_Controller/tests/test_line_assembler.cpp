/**
 * @file    test_line_assembler.cpp
 * @brief   Host unit tests for the bounded line assembler.
 */
#include "line_assembler.hpp"
#include "test_framework.hpp"

#include <cstring>

using app::LineAssembler;
using app::LineStatus;

namespace {
// Feed every char of @p s; return the status of the final byte.
template <std::size_t N>
LineStatus feedAll(LineAssembler<N>& a, const char* s) {
    LineStatus st = LineStatus::Incomplete;
    for (const char* p = s; *p != '\0'; ++p) {
        st = a.feed(static_cast<std::uint8_t>(*p));
    }
    return st;
}
} // namespace

TEST_CASE(line_completes_on_newline) {
    LineAssembler<16> a;
    EXPECT_TRUE(feedAll(a, "R30") == LineStatus::Incomplete);
    EXPECT_TRUE(a.feed('\n') == LineStatus::Ready);
    EXPECT_EQ(a.length(), 3U);
    EXPECT_TRUE(std::strcmp(a.line(), "R30") == 0);
}

TEST_CASE(line_completes_on_carriage_return) {
    LineAssembler<16> a;
    feedAll(a, "STATUS");
    EXPECT_TRUE(a.feed('\r') == LineStatus::Ready);
    EXPECT_TRUE(std::strcmp(a.line(), "STATUS") == 0);
}

TEST_CASE(line_ignores_blank_lines) {
    LineAssembler<16> a;
    EXPECT_TRUE(a.feed('\r') == LineStatus::Incomplete);
    EXPECT_TRUE(a.feed('\n') == LineStatus::Incomplete); // bare CRLF -> nothing
}

TEST_CASE(line_resumes_after_terminator) {
    LineAssembler<16> a;
    feedAll(a, "G20");
    EXPECT_TRUE(a.feed('\n') == LineStatus::Ready);
    EXPECT_TRUE(std::strcmp(a.line(), "G20") == 0);
    // Next line builds independently.
    feedAll(a, "STOP");
    EXPECT_TRUE(a.feed('\r') == LineStatus::Ready);
    EXPECT_TRUE(std::strcmp(a.line(), "STOP") == 0);
}

TEST_CASE(line_overflow_is_reported_and_discarded) {
    LineAssembler<4> a; // capacity 4 chars
    // 5 chars before terminator -> overflow.
    a.feed('A'); a.feed('B'); a.feed('C'); a.feed('D');
    EXPECT_TRUE(a.feed('E') == LineStatus::Incomplete); // marks overflow
    EXPECT_TRUE(a.feed('\n') == LineStatus::Overflow);  // reported on terminate
    // Assembler recovers for the next line.
    a.feed('O'); a.feed('K');
    EXPECT_TRUE(a.feed('\n') == LineStatus::Ready);
    EXPECT_TRUE(std::strcmp(a.line(), "OK") == 0);
}

TEST_CASE(line_exactly_capacity_is_ok) {
    LineAssembler<4> a;
    a.feed('A'); a.feed('B'); a.feed('C'); a.feed('D');
    EXPECT_TRUE(a.feed('\n') == LineStatus::Ready);
    EXPECT_EQ(a.length(), 4U);
}
