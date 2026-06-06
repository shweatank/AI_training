/**
 * @file    command_processor.cpp
 * @brief   Implementation of the command processor / responder.
 */
#include "command_processor.hpp"

#include "command_parser.hpp"
#include "format.hpp"

namespace app {

const char* CommandProcessor::stateName(TrafficState state) noexcept {
    switch (state) {
        case TrafficState::Red:    return "RED";
        case TrafficState::Green:  return "GREEN";
        case TrafficState::Yellow: return "YELLOW";
        case TrafficState::Idle:   default: return "IDLE";
    }
}

void CommandProcessor::ack() noexcept  { uart_.writeString("ACK\r\n"); }
void CommandProcessor::nack() noexcept { uart_.writeString("NACK\r\n"); }

bool CommandProcessor::apply(const Command& cmd, bool& valid) noexcept {
    valid = true;
    switch (cmd.type) {
        case CommandType::SetRed:    light_.setRedDuration(cmd.value);    return true;
        case CommandType::SetGreen:  light_.setGreenDuration(cmd.value);  return true;
        case CommandType::SetYellow: light_.setYellowDuration(cmd.value); return true;
        case CommandType::Start:     light_.start();                      return true;
        case CommandType::Stop:      light_.stop();                       return true;
        case CommandType::Status:    return false; // valid, but handled separately
        case CommandType::Help:      return false; // valid, but handled separately
        case CommandType::Invalid:   default: valid = false; return false;
    }
}

void CommandProcessor::handleLine(const char* line, std::size_t len) noexcept {
    const Command cmd = CommandParser::parse(line, len);

    bool       valid          = false;
    const bool echo_status    = apply(cmd, valid);

    if (!valid) {
        nack();
        return;
    }

    ack();

    if (cmd.type == CommandType::Status) {
        printStatus();
    } else if (cmd.type == CommandType::Help) {
        printHelp();
    } else if (echo_status) {
        // R/G/Y/START/STOP changed state -> echo the updated STATUS line.
        printStatus();
    }
}

void CommandProcessor::printStatus() noexcept {
    const TrafficConfig& cfg = light_.config();

    char buf[96];
    std::size_t pos = 0;
    char num[12];

    pos = app::fmt::append(buf, sizeof(buf), pos, "STATUS STATE=");
    pos = app::fmt::append(buf, sizeof(buf), pos, stateName(light_.state()));

    pos = app::fmt::append(buf, sizeof(buf), pos, " RUN=");
    pos = app::fmt::append(buf, sizeof(buf), pos, light_.isRunning() ? "1" : "0");

    app::fmt::toDecimal(cfg.red_sec, num, sizeof(num));
    pos = app::fmt::append(buf, sizeof(buf), pos, " R=");
    pos = app::fmt::append(buf, sizeof(buf), pos, num);

    app::fmt::toDecimal(cfg.green_sec, num, sizeof(num));
    pos = app::fmt::append(buf, sizeof(buf), pos, " G=");
    pos = app::fmt::append(buf, sizeof(buf), pos, num);

    app::fmt::toDecimal(cfg.yellow_sec, num, sizeof(num));
    pos = app::fmt::append(buf, sizeof(buf), pos, " Y=");
    pos = app::fmt::append(buf, sizeof(buf), pos, num);

    app::fmt::toDecimal(light_.remainingSec(), num, sizeof(num));
    pos = app::fmt::append(buf, sizeof(buf), pos, " REMAIN=");
    pos = app::fmt::append(buf, sizeof(buf), pos, num);

    app::fmt::append(buf, sizeof(buf), pos, "\r\n");
    uart_.writeString(buf);
}

void CommandProcessor::printHelp() noexcept {
    uart_.writeString("Commands:\r\n");
    uart_.writeString("  R<sec>  set RED duration (e.g. R30)\r\n");
    uart_.writeString("  G<sec>  set GREEN duration (e.g. G20)\r\n");
    uart_.writeString("  Y<sec>  set YELLOW duration (e.g. Y5)\r\n");
    uart_.writeString("  START   begin cycling\r\n");
    uart_.writeString("  STOP    halt (IDLE)\r\n");
    uart_.writeString("  STATUS  report timings and state\r\n");
    uart_.writeString("  HELP    this message\r\n");
}

void CommandProcessor::printBanner() noexcept {
    uart_.writeString("STM32F407 Smart Traffic Light Controller (C++17)\r\n");
    uart_.writeString("Type HELP for commands.\r\n");
}

} // namespace app
