/**
 * @file unit_test.h
 * @brief Simple Unit Test Framework
 * @author Embedded Developer
 * @version 1.0
 */

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace test {

/**
 * @brief Test result structure
 */
struct TestResult {
    const char* test_name;
    bool passed;
    const char* message;
};

/**
 * @brief Unit test framework
 */
class UnitTest {
public:
    /**
     * @brief Initialize test framework
     */
    static void init();

    /**
     * @brief Print test header
     * @param test_name Name of the test
     */
    static void startTest(const char* test_name);

    /**
     * @brief Print test result
     * @param passed Whether test passed
     * @param message Test message
     */
    static void endTest(bool passed, const char* message = "");

    /**
     * @brief Assert condition is true
     * @param condition Condition to check
     * @param message Assertion message
     * @return true if assertion passed
     */
    static bool assertEqual(bool condition, const char* message);

    /**
     * @brief Assert two integers are equal
     * @param expected Expected value
     * @param actual Actual value
     * @param message Assertion message
     * @return true if assertion passed
     */
    static bool assertEqual(int32_t expected, int32_t actual, const char* message);

    /**
     * @brief Assert two values are not equal
     * @param expected Expected value
     * @param actual Actual value
     * @param message Assertion message
     * @return true if assertion passed
     */
    static bool assertNotEqual(int32_t expected, int32_t actual, const char* message);

    /**
     * @brief Print separator
     */
    static void printSeparator();

    /**
     * @brief Print test summary
     */
    static void printSummary();

    /**
     * @brief Get total test count
     * @return Number of tests run
     */
    static uint32_t getTestCount();

    /**
     * @brief Get passed test count
     * @return Number of passed tests
     */
    static uint32_t getPassedCount();

    /**
     * @brief Get failed test count
     * @return Number of failed tests
     */
    static uint32_t getFailedCount();

private:
    static uint32_t test_count_;
    static uint32_t passed_count_;
    static uint32_t failed_count_;
    static const char* current_test_;
};

} // namespace test

// Convenience macros for testing
#define TEST_ASSERT(condition) \
    test::UnitTest::assertEqual(condition, #condition)

#define TEST_ASSERT_EQUAL(expected, actual) \
    test::UnitTest::assertEqual(expected, actual, #expected " == " #actual)

#define TEST_ASSERT_NOT_EQUAL(expected, actual) \
    test::UnitTest::assertNotEqual(expected, actual, #expected " != " #actual)

#endif // UNIT_TEST_H
