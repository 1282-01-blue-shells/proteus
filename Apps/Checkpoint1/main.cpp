#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"


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


// Function definitions

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

void waitForLight() {
    Debugger::printLine(9, "Waiting for light...");

    // wait for light
    while (lightSensor.Value()>lsThreshold); // if no light do nothing


    // Change font color to yellow
    Debugger::setFontColor(0xFFFF00);

    // Print funny message
    Debugger::printWrap(10, "NOW AT LAST I SEEEE THE LIGHT");

    // Reset font color
    Debugger::setFontColor();
}

// Drives to kiosk, then backs up down the ramp
void navigate() {
    goToKiosk();
    backDownTheRamp();
}

void goToKiosk() {
    Debugger::printNextLine("starting");
    Motors::drive(2);
    Motors::turn(-20);
    Motors::drive(2);
    Motors::turn(-20);

    Debugger::printNextLine("going forward");
    Motors::drive(30);

    Debugger::printNextLine("turning left");
    Motors::turn(-45);

    Debugger::printNextLine("going forward");
    Motors::drive(12);

    Debugger::printNextLine("turning right");
    Motors::turn(45);

    Debugger::printNextLine("going forward to kiosk");
    Motors::start(true);
    Debugger::sleep(2.f);

    Motors::stop();
    Debugger::printNextLine("done");
}

void backDownTheRamp() {
    Motors::drive(-distanceToBackUp);
    Motors::turn(-45);
    Motors::drive(-12);
    Motors::turn(45);
    Motors::drive(-24);
}

void motorTest() {
    Debugger::printLine(1, "left motor forward...");
    Motors::lMotor.SetPercent(40);
    Debugger::sleep(3);

    Debugger::printLine(2, "left motor backward...");
    Motors::lMotor.SetPercent(-40);
    Debugger::sleep(3);

    Motors::lMotor.Stop();

    Debugger::printLine(3, "right motor forward...");
    Motors::rMotor.SetPercent(40);
    Debugger::sleep(3);

    Debugger::printLine(4, "right motor backward...");
    Motors::rMotor.SetPercent(-40);
    Debugger::sleep(3);

    Motors::rMotor.Stop();

    Debugger::printLine(5, "done.");
}

void encoderTest() {
    Debugger::printLine(1, "left motor forward...");
    Motors::lEncoder.ResetCounts();
    Motors::lMotor.SetPercent(40);
    double startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        Debugger::printLine(2, "encoder reading: %i", Motors::lEncoder.Counts());
        Debugger::sleep(0.01f);
    }

    Debugger::printLine(3, "left motor backward...");
    Motors::lEncoder.ResetCounts();
    Motors::lMotor.SetPercent(-40);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        Debugger::printLine(4, "encoder reading: %i", Motors::lEncoder.Counts());
        Debugger::sleep(0.01f);
    }
    Motors::lMotor.Stop();

    Debugger::printLine(5, "right motor forward...");
    Motors::rEncoder.ResetCounts();
    Motors::rMotor.SetPercent(40);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        Debugger::printLine(6, "encoder reading: %i", Motors::rEncoder.Counts());
        Debugger::sleep(0.01f);
    }

    Debugger::printLine(7, "right motor backward...");
    Motors::rEncoder.ResetCounts();
    Motors::rMotor.SetPercent(-40);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        Debugger::printLine(8, "encoder reading: %i", Motors::rEncoder.Counts());
        Debugger::sleep(0.01f);
    }
    Motors::rMotor.Stop();
}