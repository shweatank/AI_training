/**
 * @file led_manager_test.cpp
 * @brief LED Manager Unit Tests Implementation
 */

#include "led_manager_test.h"

namespace test {

const char* LEDManagerTest::TEST_SUITE_NAME = "LED Manager Tests";

void LEDManagerTest::runAllTests() {
    UnitTest::init();
    UnitTest::printSeparator();
    printf("Running %s\n", TEST_SUITE_NAME);
    UnitTest::printSeparator();

    testInit();
    testTurnOn();
    testTurnOff();
    testToggle();
    testBlinkOnce();
    testGetLEDPin();
    testGetLEDCount();
    testAllLEDColors();
    testBlinkAll();
    testBlinkAllSimultaneous();

    UnitTest::printSeparator();
    UnitTest::printSummary();
    UnitTest::printSeparator();
}

void LEDManagerTest::testInit() {
    UnitTest::startTest("LED Manager Initialization");

    GPIOMock::resetMock();
    bool result = app::LEDManager::init();

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "LED Manager initialized successfully");
}

void LEDManagerTest::testTurnOn() {
    UnitTest::startTest("LED Turn On");

    GPIOMock::resetMock();
    app::LEDManager::init();

    bool result = app::LEDManager::turnOn(app::LEDColor::GREEN);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "LED turned on successfully");
}

void LEDManagerTest::testTurnOff() {
    UnitTest::startTest("LED Turn Off");

    GPIOMock::resetMock();
    app::LEDManager::init();

    app::LEDManager::turnOn(app::LEDColor::GREEN);
    bool result = app::LEDManager::turnOff(app::LEDColor::GREEN);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "LED turned off successfully");
}

void LEDManagerTest::testToggle() {
    UnitTest::startTest("LED Toggle");

    GPIOMock::resetMock();
    app::LEDManager::init();

    bool result1 = app::LEDManager::toggle(app::LEDColor::ORANGE);
    bool result2 = app::LEDManager::toggle(app::LEDColor::ORANGE);

    bool passed = TEST_ASSERT(result1) && TEST_ASSERT(result2);

    UnitTest::endTest(passed, "LED toggled successfully");
}

void LEDManagerTest::testBlinkOnce() {
    UnitTest::startTest("LED Blink Once");

    GPIOMock::resetMock();
    app::LEDManager::init();

    bool result = app::LEDManager::blinkOnce(app::LEDColor::RED, 50);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "LED blinked once successfully");
}

void LEDManagerTest::testBlinkAll() {
    UnitTest::startTest("LED Blink All Sequentially");

    GPIOMock::resetMock();
    app::LEDManager::init();

    bool result = app::LEDManager::blinkAll(1, 50);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "All LEDs blinked sequentially");
}

void LEDManagerTest::testBlinkAllSimultaneous() {
    UnitTest::startTest("LED Blink All Simultaneously");

    GPIOMock::resetMock();
    app::LEDManager::init();

    bool result = app::LEDManager::blinkAllSimultaneous(1, 50);

    bool passed = TEST_ASSERT(result);

    UnitTest::endTest(passed, "All LEDs blinked simultaneously");
}

void LEDManagerTest::testGetLEDPin() {
    UnitTest::startTest("Get LED Pin");

    uint8_t green_pin = app::LEDManager::getLEDPin(app::LEDColor::GREEN);
    uint8_t orange_pin = app::LEDManager::getLEDPin(app::LEDColor::ORANGE);
    uint8_t red_pin = app::LEDManager::getLEDPin(app::LEDColor::RED);
    uint8_t blue_pin = app::LEDManager::getLEDPin(app::LEDColor::BLUE);

    bool passed = TEST_ASSERT_EQUAL(12, green_pin) &&
                  TEST_ASSERT_EQUAL(13, orange_pin) &&
                  TEST_ASSERT_EQUAL(14, red_pin) &&
                  TEST_ASSERT_EQUAL(15, blue_pin);

    UnitTest::endTest(passed, "All LED pins returned correctly");
}

void LEDManagerTest::testGetLEDCount() {
    UnitTest::startTest("Get LED Count");

    uint8_t count = app::LEDManager::getLEDCount();

    bool passed = TEST_ASSERT_EQUAL(4, count);

    UnitTest::endTest(passed, "LED count is 4");
}

void LEDManagerTest::testAllLEDColors() {
    UnitTest::startTest("Control All LED Colors");

    GPIOMock::resetMock();
    app::LEDManager::init();

    app::LEDColor colors[] = {
        app::LEDColor::GREEN,
        app::LEDColor::ORANGE,
        app::LEDColor::RED,
        app::LEDColor::BLUE
    };

    bool passed = true;
    for (const auto& color : colors) {
        if (!app::LEDManager::turnOn(color)) {
            passed = false;
            break;
        }
        if (!app::LEDManager::turnOff(color)) {
            passed = false;
            break;
        }
    }

    TEST_ASSERT(passed);

    UnitTest::endTest(passed, "All LED colors controlled successfully");
}

void LEDManagerTest::mockUARTInit() {
    // Mock UART initialization for testing
    // In real implementation, UART would communicate with hardware
}

} // namespace test
