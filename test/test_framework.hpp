// test_framework.hpp — tiny zero-dependency test runner shared by all suites.
//
// Goal: make each test case VISIBLE in the terminal. Every RUN() prints the
// case number, its name, a one-line description of what it checks, and a
// PASS/FAIL line — so a green run is something you can actually watch, not a
// silent success. Uses C++17 inline variables so it stays header-only.
#pragma once

#include <cstdint>
#include <iostream>

namespace tf {

inline int  g_total        = 0;   // checks across the whole suite
inline int  g_total_failed = 0;
inline int  g_case_checks  = 0;   // checks in the current case
inline int  g_case_failed  = 0;
inline int  g_case_index   = 0;

inline void check(bool cond, const char* expr, const char* file, int line) {
    ++g_case_checks; ++g_total;
    if (!cond) {
        ++g_case_failed; ++g_total_failed;
        std::cout << "        ✗ assert failed: " << expr
                  << "  (" << file << ':' << line << ")\n";
    }
}

inline void check_eq(std::uint32_t got, std::uint32_t want,
                     const char* expr, const char* file, int line) {
    ++g_case_checks; ++g_total;
    if (got != want) {
        ++g_case_failed; ++g_total_failed;
        std::cout << std::hex
                  << "        ✗ " << expr << "  (got 0x" << got
                  << ", want 0x" << want << ")  (" << file << ':' << line << ")\n"
                  << std::dec;
    }
}

// Run one test case, narrating it to the terminal.
inline void run(const char* name, void (*fn)(), const char* desc) {
    ++g_case_index;
    g_case_checks = 0;
    g_case_failed = 0;
    std::cout << "  [" << g_case_index << "] " << name << "\n"
              << "      → " << desc << "\n";
    fn();
    if (g_case_failed == 0) {
        std::cout << "      ✓ PASS  (" << g_case_checks << " checks)\n\n";
    } else {
        std::cout << "      ✗ FAIL  (" << g_case_failed << " of "
                  << g_case_checks << " checks failed)\n\n";
    }
}

inline void banner(const char* title) {
    std::cout << "\n========================================================\n"
              << "  " << title << "\n"
              << "========================================================\n\n";
}

inline int summary(const char* suite) {
    std::cout << "--------------------------------------------------------\n"
              << "  " << suite << ": " << g_case_index << " test cases, "
              << (g_total - g_total_failed) << "/" << g_total << " checks passed.\n";
    if (g_total_failed) {
        std::cout << "  RESULT: ✗ " << g_total_failed << " CHECK(S) FAILED\n";
        return 1;
    }
    std::cout << "  RESULT: ✓ ALL TESTS PASSED\n";
    return 0;
}

} // namespace tf

#define CHECK(cond)        tf::check((cond), #cond, __FILE__, __LINE__)
#define CHECK_EQ(got, exp) tf::check_eq((got), (exp), #got " == " #exp, __FILE__, __LINE__)
#define RUN(fn, desc)      tf::run(#fn, (fn), (desc))
