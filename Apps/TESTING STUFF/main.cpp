#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"
#include "FEHServo.h"

// Function declaration

int main();
void runSection();
void waitForLight();
void navigate();
void goToKiosk();
void backDownTheRamp();
void motorTest();
void encoderTest();

// Variables

AnalogInputPin lightSensor(FEHIO::P0_7);

float distanceToBackUp = 8.f;
float lsThreshold = 2.f;

FEHServo r2d2Servo(FEHServo::Servo1);

int main() {
    ProteOS::registerVariable("distanceToBackUp", &distanceToBackUp);
    ProteOS::registerVariable("lsThreshold", &lsThreshold);

    ProteOS::registerFunction("motorTest()", &motorTest);
    ProteOS::registerFunction("encoderTest()", &encoderTest);
    ProteOS::registerFunction("waitForLight()", &waitForLight);
    ProteOS::registerFunction("goToKiosk()", &goToKiosk);
    ProteOS::registerFunction("backDownTheRamp()", &backDownTheRamp);
    ProteOS::registerFunction("navigate()", &navigate);
    ProteOS::registerFunction("runSection()", &runSection);

    ProteOS::run();
}


void runSection() {
    waitForLight();
    navigate();
}