/**
 * @file    test_command_parser.cpp
 * @brief   Host unit tests for the command parser (valid + invalid + bounds).
 */
#include "command_parser.hpp"
#include "test_framework.hpp"

#include <cstring>

using app::Command;
using app::CommandParser;
using app::CommandType;

namespace {
Command parse(const char* s) {
    return CommandParser::parse(s, std::strlen(s));
}
} // namespace

TEST_CASE(parse_set_red_green_yellow) {
    Command r = parse("R30");
    EXPECT_TRUE(r.type == CommandType::SetRed);
    EXPECT_EQ(r.value, 30U);

    Command g = parse("G20");
    EXPECT_TRUE(g.type == CommandType::SetGreen);
    EXPECT_EQ(g.value, 20U);

    Command y = parse("Y5");
    EXPECT_TRUE(y.type == CommandType::SetYellow);
    EXPECT_EQ(y.value, 5U);
}

TEST_CASE(parse_keyword_commands) {
    EXPECT_TRUE(parse("START").type  == CommandType::Start);
    EXPECT_TRUE(parse("STOP").type   == CommandType::Stop);
    EXPECT_TRUE(parse("STATUS").type == CommandType::Status);
    EXPECT_TRUE(parse("HELP").type   == CommandType::Help);
}

TEST_CASE(parse_is_case_insensitive) {
    EXPECT_TRUE(parse("start").type == CommandType::Start);
    EXPECT_TRUE(parse("Stop").type  == CommandType::Stop);
    Command r = parse("r45");
    EXPECT_TRUE(r.type == CommandType::SetRed);
    EXPECT_EQ(r.value, 45U);
}

TEST_CASE(parse_trims_surrounding_whitespace) {
    Command r = parse("  R30  ");
    EXPECT_TRUE(r.type == CommandType::SetRed);
    EXPECT_EQ(r.value, 30U);
    EXPECT_TRUE(parse("\tSTART ").type == CommandType::Start);
}

TEST_CASE(parse_rejects_unknown_tokens) {
    EXPECT_TRUE(parse("FOO").type   == CommandType::Invalid);
    EXPECT_TRUE(parse("X10").type   == CommandType::Invalid);
    EXPECT_TRUE(parse("STARTX").type == CommandType::Invalid);
    EXPECT_TRUE(parse("").type      == CommandType::Invalid);
    EXPECT_TRUE(parse("   ").type   == CommandType::Invalid);
}

TEST_CASE(parse_rejects_malformed_durations) {
    EXPECT_TRUE(parse("R").type    == CommandType::Invalid); // no digits
    EXPECT_TRUE(parse("Rabc").type == CommandType::Invalid); // non-digit
    EXPECT_TRUE(parse("R1x").type  == CommandType::Invalid); // trailing junk
    EXPECT_TRUE(parse("R-5").type  == CommandType::Invalid); // sign not allowed
}

TEST_CASE(parse_enforces_duration_bounds) {
    // Below minimum (0 < kMinDurationSec).
    EXPECT_TRUE(parse("R0").type == CommandType::Invalid);
    // At maximum boundary is accepted.
    Command hi = parse("R3600");
    EXPECT_TRUE(hi.type == CommandType::SetRed);
    EXPECT_EQ(hi.value, 3600U);
    // Above maximum rejected.
    EXPECT_TRUE(parse("R3601").type   == CommandType::Invalid);
    EXPECT_TRUE(parse("R99999").type  == CommandType::Invalid);
    // Huge value that would overflow uint32 also rejected, not wrapped.
    EXPECT_TRUE(parse("R99999999999").type == CommandType::Invalid);
}

TEST_CASE(parse_min_duration_accepted) {
    Command y = parse("Y1");
    EXPECT_TRUE(y.type == CommandType::SetYellow);
    EXPECT_EQ(y.value, 1U);
}
