/**
 * @file unit_test.cpp
 * @brief Unit Test Framework Implementation
 */

#include "unit_test.h"

namespace test {

uint32_t UnitTest::test_count_ = 0;
uint32_t UnitTest::passed_count_ = 0;
uint32_t UnitTest::failed_count_ = 0;
const char* UnitTest::current_test_ = "";

void UnitTest::init() {
    test_count_ = 0;
    passed_count_ = 0;
    failed_count_ = 0;
    current_test_ = "";
}

void UnitTest::startTest(const char* test_name) {
    current_test_ = test_name;
    printf("[TEST] %s... ", test_name);
}

void UnitTest::endTest(bool passed, const char* message) {
    test_count_++;

    if (passed) {
        passed_count_++;
        printf("PASSED");
    } else {
        failed_count_++;
        printf("FAILED");
    }

    if (message && message[0] != '\0') {
        printf(" (%s)", message);
    }

    printf("\n");
}

bool UnitTest::assertEqual(bool condition, const char* message) {
    if (!condition) {
        printf("\n  Assertion failed: %s\n", message);
        return false;
    }
    return true;
}

bool UnitTest::assertEqual(int32_t expected, int32_t actual, const char* message) {
    if (expected != actual) {
        printf("\n  Assertion failed: %s\n", message);
        printf("  Expected: %d, Got: %d\n", expected, actual);
        return false;
    }
    return true;
}

bool UnitTest::assertNotEqual(int32_t expected, int32_t actual, const char* message) {
    if (expected == actual) {
        printf("\n  Assertion failed: %s\n", message);
        printf("  Expected: %d, Got: %d\n", expected, actual);
        return false;
    }
    return true;
}

void UnitTest::printSeparator() {
    printf("========================================\n");
}

void UnitTest::printSummary() {
    printf("Test Summary:\n");
    printf("  Total:  %d\n", test_count_);
    printf("  Passed: %d\n", passed_count_);
    printf("  Failed: %d\n", failed_count_);
    printf("  Rate:   %.1f%%\n", 
           test_count_ > 0 ? (passed_count_ * 100.0f / test_count_) : 0.0f);
}

uint32_t UnitTest::getTestCount() {
    return test_count_;
}

uint32_t UnitTest::getPassedCount() {
    return passed_count_;
}

uint32_t UnitTest::getFailedCount() {
    return failed_count_;
}

} // namespace test
