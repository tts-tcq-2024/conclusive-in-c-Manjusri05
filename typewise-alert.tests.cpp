#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "typewise-alert.h"

// Test for getTemperatureLimits function
void test_getTemperatureLimits() {
    TemperatureLimits passiveLimits = getTemperatureLimits(PASSIVE_COOLING);
    assert(passiveLimits.lowerLimit == 0);
    assert(passiveLimits.upperLimit == 35);

    TemperatureLimits hiActiveLimits = getTemperatureLimits(HI_ACTIVE_COOLING);
    assert(hiActiveLimits.lowerLimit == 0);
    assert(hiActiveLimits.upperLimit == 45);

    TemperatureLimits medActiveLimits = getTemperatureLimits(MED_ACTIVE_COOLING);
    assert(medActiveLimits.lowerLimit == 0);
    assert(medActiveLimits.upperLimit == 40);
}

// Test for checkBreach function
void test_checkBreach() {
    assert(checkBreach(25, 0, 35) == NORMAL);
    assert(checkBreach(-1, 0, 35) == TOO_LOW);
    assert(checkBreach(36, 0, 35) == TOO_HIGH);
}

// Test for classifyTemperatureBreach function
void test_classifyTemperatureBreach() {
    assert(classifyTemperatureBreach(PASSIVE_COOLING, 20) == NORMAL);
    assert(classifyTemperatureBreach(PASSIVE_COOLING, -5) == TOO_LOW);
    assert(classifyTemperatureBreach(PASSIVE_COOLING, 40) == TOO_HIGH);

    assert(classifyTemperatureBreach(HI_ACTIVE_COOLING, 20) == NORMAL);
    assert(classifyTemperatureBreach(HI_ACTIVE_COOLING, -5) == TOO_LOW);
    assert(classifyTemperatureBreach(HI_ACTIVE_COOLING, 50) == TOO_HIGH);

    assert(classifyTemperatureBreach(MED_ACTIVE_COOLING, 20) == NORMAL);
    assert(classifyTemperatureBreach(MED_ACTIVE_COOLING, -5) == TOO_LOW);
    assert(classifyTemperatureBreach(MED_ACTIVE_COOLING, 45) == TOO_HIGH);
}

// Mock function to capture output for testing sendToController and sendToEmail
void test_sendToController() {
    // Redirect stdout to capture output
    freopen("temp_output.txt", "w", stdout);

    sendToController(TOO_LOW);
    fclose(stdout);
    FILE *fp = fopen("temp_output.txt", "r");
    char output[100];
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "feed : 1\n") == 0); // Expected for TOO_LOW (1)

    freopen("temp_output.txt", "w", stdout);
    sendToController(TOO_HIGH);
    fclose(stdout);
    fp = fopen("temp_output.txt", "r");
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "feed : 2\n") == 0); // Expected for TOO_HIGH (2)

    freopen("temp_output.txt", "w", stdout);
    sendToController(NORMAL);
    fclose(stdout);
    fp = fopen("temp_output.txt", "r");
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "feed : 0\n") == 0); // Expected for NORMAL (0)
}

void test_sendToEmail() {
    freopen("temp_output.txt", "w", stdout);

    sendToEmail(TOO_LOW);
    fclose(stdout);
    FILE *fp = fopen("temp_output.txt", "r");
    char output[100];
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "To: a.b@c.com\nHi, the temperature is too low\n") == 0);

    freopen("temp_output.txt", "w", stdout);
    sendToEmail(TOO_HIGH);
    fclose(stdout);
    fp = fopen("temp_output.txt", "r");
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "To: a.b@c.com\nHi, the temperature is too high\n") == 0);

    freopen("temp_output.txt", "w", stdout);
    sendToEmail(NORMAL);
    fclose(stdout);
    fp = fopen("temp_output.txt", "r");
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "") == 0); // No output expected for NORMAL
}

void test_checkAndAlert() {
    BatteryCharacter batteryChar = {PASSIVE_COOLING};

    // Test alert to controller
    freopen("temp_output.txt", "w", stdout);
    checkAndAlert(TO_CONTROLLER, batteryChar, 40);
    fclose(stdout);
    FILE *fp = fopen("temp_output.txt", "r");
    char output[100];
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "feed : 2\n") == 0); // TOO_HIGH for PASSIVE_COOLING

    // Test alert to email
    freopen("temp_output.txt", "w", stdout);
    checkAndAlert(TO_EMAIL, batteryChar, -5);
    fclose(stdout);
    fp = fopen("temp_output.txt", "r");
    fgets(output, sizeof(output), fp);
    assert(strcmp(output, "To: a.b@c.com\nHi, the temperature is too low\n") == 0);
}

int main() {
    test_getTemperatureLimits();
    test_checkBreach();
    test_classifyTemperatureBreach();
    test_sendToController();
    test_sendToEmail();
    test_checkAndAlert();

    printf("All tests passed!\n");
    return 0;
}
