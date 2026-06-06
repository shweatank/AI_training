/**
 * @file test_runner.cpp
 * @brief Test Runner - Executes all unit tests
 * @author Embedded Developer
 * @version 1.0
 */

#include <cstdio>
#include "gpio_test.h"
#include "led_manager_test.h"

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║   STM32F407 Embedded System Tests      ║\n");
    printf("║   C++17 Unit Test Suite                ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    // Run GPIO tests
    test::GPIOTest::runAllTests();

    printf("\n");

    // Run LED Manager tests
    test::LEDManagerTest::runAllTests();

    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║   All Tests Completed                  ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    return 0;
}
