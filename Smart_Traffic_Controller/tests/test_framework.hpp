/**
 * @file    test_framework.hpp
 * @brief   Tiny header-only unit-test framework (zero external dependencies).
 *
 * Just enough to register cases, run assertions, and report a pass/fail summary
 * with a non-zero exit code on failure (so `make test` fails CI). Drop-in
 * replaceable with GoogleTest/Catch2 if desired.
 */
#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace tf {

struct TestCase {
    std::string name;
    std::function<void()> fn;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> cases;
    return cases;
}

inline int& failures() {
    static int n = 0;
    return n;
}

inline int& checks() {
    static int n = 0;
    return n;
}

struct Registrar {
    Registrar(const std::string& name, std::function<void()> fn) {
        registry().push_back({name, std::move(fn)});
    }
};

inline void reportFailure(const char* file, int line, const std::string& msg) {
    ++failures();
    std::printf("    [FAIL] %s:%d  %s\n", file, line, msg.c_str());
}

inline int runAll() {
    int failed_cases = 0;
    for (auto& tc : registry()) {
        const int before = failures();
        std::printf("[ RUN  ] %s\n", tc.name.c_str());
        tc.fn();
        if (failures() == before) {
            std::printf("[  OK  ] %s\n", tc.name.c_str());
        } else {
            std::printf("[FAILED] %s\n", tc.name.c_str());
            ++failed_cases;
        }
    }
    std::printf("\n========================================\n");
    std::printf("Cases: %zu   Checks: %d   Failures: %d\n",
                registry().size(), checks(), failures());
    std::printf("Result: %s\n", failed_cases == 0 ? "PASS" : "FAIL");
    std::printf("========================================\n");
    return failed_cases == 0 ? 0 : 1;
}

} // namespace tf

// --- Macros ---------------------------------------------------------------
#define TEST_CASE(name)                                                        \
    static void name();                                                        \
    static tf::Registrar reg_##name(#name, name);                              \
    static void name()

#define EXPECT_TRUE(cond)                                                      \
    do {                                                                       \
        ++tf::checks();                                                        \
        if (!(cond)) tf::reportFailure(__FILE__, __LINE__,                     \
                                       "EXPECT_TRUE(" #cond ")");              \
    } while (0)

#define EXPECT_FALSE(cond) EXPECT_TRUE(!(cond))

#define EXPECT_EQ(a, b)                                                        \
    do {                                                                       \
        ++tf::checks();                                                        \
        auto _va = (a);                                                        \
        auto _vb = (b);                                                        \
        if (!(_va == _vb)) {                                                   \
            char _buf[160];                                                    \
            std::snprintf(_buf, sizeof(_buf),                                  \
                          "EXPECT_EQ(" #a ", " #b ")  got 0x%lX vs 0x%lX",     \
                          static_cast<unsigned long>(_va),                     \
                          static_cast<unsigned long>(_vb));                    \
            tf::reportFailure(__FILE__, __LINE__, _buf);                       \
        }                                                                      \
    } while (0)

#define RUN_ALL_TESTS() tf::runAll()

#endif // TEST_FRAMEWORK_HPP
