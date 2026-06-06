/**
 * @file gpio_test.cpp
 * @brief GPIO Driver Unit Tests Implementation
 */

#include "gpio_test.h"

namespace test {

const char* GPIOTest::TEST_SUITE_NAME = "GPIO Driver Tests";

void GPIOTest::runAllTests() {
    UnitTest::init();
    UnitTest::printSeparator();
    printf("Running %s\n", TEST_SUITE_NAME);
    UnitTest::printSeparator();

    testInit();
    testConfigurePin();
    testSetPinHigh();
    testSetPinLow();
    testTogglePin();
    testReadPin();
    testSetPins();
    testResetPins();
    testInvalidPin();
    testModeConfiguration();
    testSpeedConfiguration();
    testPullConfiguration();

    UnitTest::printSeparator();
    UnitTest::printSummary();
    UnitTest::printSeparator();
}

void GPIOTest::testInit() {
    UnitTest::startTest("GPIO Initialization");

    GPIOMock::resetMock();
    bool result = driver::GPIO::init();

    bool passed = TEST_ASSERT(result) && 
                  TEST_ASSERT(GPIOMock::isClockEnabled());

    UnitTest::endTest(passed, "GPIO initialized and clock enabled");
}

void GPIOTest::testConfigurePin() {
    UnitTest::startTest("GPIO Pin Configuration");

    GPIOMock::resetMock();
    driver::GPIO::init();

    bool result = driver::GPIO::configurePin(12, 
                                            driver::GPIOMode::OUTPUT,
                                            driver::GPIOSpeed::FAST,
                                            driver::GPIOPull::NO_PULL);

    bool passed = TEST_ASSERT(result) &&
                  TEST_ASSERT_EQUAL(1, GPIOMock::getMODERValue(12)) &&
                  TEST_ASSERT_EQUAL(2, GPIOMock::getOSPEEDRValue(12)) &&
                  TEST_ASSERT_EQUAL(0, GPIOMock::getPUPDRValue(12));

    UnitTest::endTest(passed, "Pin configured as output with correct settings");
}

void GPIOTest::testSetPinHigh() {
    UnitTest::startTest("Set GPIO Pin HIGH");

    GPIOMock::resetMock();
    driver::GPIO::init();
    driver::GPIO::configurePin(12, driver::GPIOMode::OUTPUT);

    bool result = driver::GPIO::setPin(12, driver::GPIOLevel::HIGH);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "Pin set to HIGH successfully");
}

void GPIOTest::testSetPinLow() {
    UnitTest::startTest("Set GPIO Pin LOW");

    GPIOMock::resetMock();
    driver::GPIO::init();
    driver::GPIO::configurePin(12, driver::GPIOMode::OUTPUT);

    // First set high
    driver::GPIO::setPin(12, driver::GPIOLevel::HIGH);

    // Then set low
    bool result = driver::GPIO::setPin(12, driver::GPIOLevel::LOW);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "Pin set to LOW successfully");
}

void GPIOTest::testTogglePin() {
    UnitTest::startTest("Toggle GPIO Pin");

    GPIOMock::resetMock();
    driver::GPIO::init();
    driver::GPIO::configurePin(12, driver::GPIOMode::OUTPUT);

    bool result1 = driver::GPIO::togglePin(12);
    bool result2 = driver::GPIO::togglePin(12);

    bool passed = TEST_ASSERT(result1) && TEST_ASSERT(result2);

    UnitTest::endTest(passed, "Pin toggled successfully");
}

void GPIOTest::testReadPin() {
    UnitTest::startTest("Read GPIO Pin");

    GPIOMock::resetMock();
    driver::GPIO::init();
    driver::GPIO::configurePin(12, driver::GPIOMode::INPUT);

    uint8_t value = driver::GPIO::readPin(12);

    bool passed = TEST_ASSERT_EQUAL(0, value);

    UnitTest::endTest(passed, "Pin read successfully");
}

void GPIOTest::testSetPins() {
    UnitTest::startTest("Set Multiple GPIO Pins");

    GPIOMock::resetMock();
    driver::GPIO::init();

    // Configure pins
    driver::GPIO::configurePin(12, driver::GPIOMode::OUTPUT);
    driver::GPIO::configurePin(13, driver::GPIOMode::OUTPUT);
    driver::GPIO::configurePin(14, driver::GPIOMode::OUTPUT);
    driver::GPIO::configurePin(15, driver::GPIOMode::OUTPUT);

    // Set multiple pins
    uint16_t mask = (1U << 12) | (1U << 13) | (1U << 14) | (1U << 15);
    bool result = driver::GPIO::setPins(mask);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "Multiple pins set successfully");
}

void GPIOTest::testResetPins() {
    UnitTest::startTest("Reset Multiple GPIO Pins");

    GPIOMock::resetMock();
    driver::GPIO::init();

    // Configure pins
    for (uint8_t i = 12; i <= 15; i++) {
        driver::GPIO::configurePin(i, driver::GPIOMode::OUTPUT);
    }

    // Set pins
    uint16_t mask = (1U << 12) | (1U << 13) | (1U << 14) | (1U << 15);
    driver::GPIO::setPins(mask);

    // Reset pins
    bool result = driver::GPIO::resetPins(mask);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "Multiple pins reset successfully");
}

void GPIOTest::testInvalidPin() {
    UnitTest::startTest("Invalid GPIO Pin Handling");

    GPIOMock::resetMock();
    driver::GPIO::init();

    // Try to configure invalid pin
    bool result = driver::GPIO::configurePin(20, driver::GPIOMode::OUTPUT);

    bool passed = TEST_ASSERT(!result);

    UnitTest::endTest(passed, "Invalid pin rejected correctly");
}

void GPIOTest::testModeConfiguration() {
    UnitTest::startTest("GPIO Mode Configuration");

    GPIOMock::resetMock();
    driver::GPIO::init();

    driver::GPIO::configurePin(12, driver::GPIOMode::OUTPUT);
    bool output_mode = TEST_ASSERT_EQUAL(1, GPIOMock::getMODERValue(12));

    driver::GPIO::configurePin(13, driver::GPIOMode::INPUT);
    bool input_mode = TEST_ASSERT_EQUAL(0, GPIOMock::getMODERValue(13));

    driver::GPIO::configurePin(14, driver::GPIOMode::ALTERNATE);
    bool alternate_mode = TEST_ASSERT_EQUAL(2, GPIOMock::getMODERValue(14));

    driver::GPIO::configurePin(15, driver::GPIOMode::ANALOG);
    bool analog_mode = TEST_ASSERT_EQUAL(3, GPIOMock::getMODERValue(15));

    bool passed = output_mode && input_mode && alternate_mode && analog_mode;

    UnitTest::endTest(passed, "All modes configured correctly");
}

void GPIOTest::testSpeedConfiguration() {
    UnitTest::startTest("GPIO Speed Configuration");

    GPIOMock::resetMock();
    driver::GPIO::init();

    driver::GPIO::configurePin(12, driver::GPIOMode::OUTPUT, driver::GPIOSpeed::SLOW);
    bool slow = TEST_ASSERT_EQUAL(0, GPIOMock::getOSPEEDRValue(12));

    driver::GPIO::configurePin(13, driver::GPIOMode::OUTPUT, driver::GPIOSpeed::MEDIUM);
    bool medium = TEST_ASSERT_EQUAL(1, GPIOMock::getOSPEEDRValue(13));

    driver::GPIO::configurePin(14, driver::GPIOMode::OUTPUT, driver::GPIOSpeed::FAST);
    bool fast = TEST_ASSERT_EQUAL(2, GPIOMock::getOSPEEDRValue(14));

    driver::GPIO::configurePin(15, driver::GPIOMode::OUTPUT, driver::GPIOSpeed::VERY_FAST);
    bool very_fast = TEST_ASSERT_EQUAL(3, GPIOMock::getOSPEEDRValue(15));

    bool passed = slow && medium && fast && very_fast;

    UnitTest::endTest(passed, "All speeds configured correctly");
}

void GPIOTest::testPullConfiguration() {
    UnitTest::startTest("GPIO Pull Configuration");

    GPIOMock::resetMock();
    driver::GPIO::init();

    driver::GPIO::configurePin(12, driver::GPIOMode::INPUT, driver::GPIOSpeed::MEDIUM, driver::GPIOPull::NO_PULL);
    bool no_pull = TEST_ASSERT_EQUAL(0, GPIOMock::getPUPDRValue(12));

    driver::GPIO::configurePin(13, driver::GPIOMode::INPUT, driver::GPIOSpeed::MEDIUM, driver::GPIOPull::PULL_UP);
    bool pull_up = TEST_ASSERT_EQUAL(1, GPIOMock::getPUPDRValue(13));

    driver::GPIO::configurePin(14, driver::GPIOMode::INPUT, driver::GPIOSpeed::MEDIUM, driver::GPIOPull::PULL_DOWN);
    bool pull_down = TEST_ASSERT_EQUAL(2, GPIOMock::getPUPDRValue(14));

    bool passed = no_pull && pull_up && pull_down;

    UnitTest::endTest(passed, "All pull configurations applied correctly");
}

} // namespace test
