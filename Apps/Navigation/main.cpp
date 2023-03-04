#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"

#define MOTOR_POWER 40

void runSection();
void waitForLight();
void navigate();
void goToKiosk();
void backDownTheRamp();
void motorTest();
void encoderTest();


AnalogInputPin lightSensor(FEHIO::P0_7);

float distanceToBackUp = 8.f;
float lsThreshold = 2.f;

int main() {
    registerIOVariable("distanceToBackUp", &distanceToBackUp);
    registerIOVariable("lsThreshold", &lsThreshold);

    registerIOFunction("motorTest()", &motorTest);
    registerIOFunction("encoderTest()", &encoderTest);
    registerIOFunction("waitForLight()", &waitForLight);
    registerIOFunction("goToKiosk()", &goToKiosk);
    registerIOFunction("backDownTheRamp()", &backDownTheRamp);
    registerIOFunction("navigate()", &navigate);
    registerIOFunction("runSection()", &runSection);

    openIOMenu();
}


void runSection() {
    waitForLight();
    navigate();
}

void waitForLight() {
    printWrapF(9, "Waiting for light...");

    //wait for light
    while (lightSensor.Value()>lsThreshold); //if no light do nothing
    setDebuggerFontColor(0xFFFF00);
    printWrapF(10, "NOW AT LAST I SEEEE THE LIGHT");
    setDebuggerFontColor();
}

void navigate() {
    goToKiosk();
    backDownTheRamp();
}

void goToKiosk() {
    printNextLineF("starting");
    Motors::drive(2);
    Motors::turn(-20);
    Motors::drive(2);
    Motors::turn(-20);

    printNextLineF("going forward");
    Motors::drive(30);

    printNextLineF("turning left");
    Motors::turn(-45);

    printNextLineF("going forward");
    Motors::drive(12);

    printNextLineF("turning right");
    Motors::turn(45);

    printNextLineF("going forward to kiosk");
    Motors::start(true);
    sleepWithAbortCheck(2.f);

    Motors::stop();
    printNextLineF("done");
}

void backDownTheRamp() {
    Motors::drive(-distanceToBackUp);
    Motors::turn(-45);
    Motors::drive(-12);
    Motors::turn(45);
    Motors::drive(-24);
}

void motorTest() {
    printLineF(1, "left motor forward...");
    Motors::lMotor.SetPercent(40);
    sleepWithAbortCheck(3);

    printLineF(2, "left motor backward...");
    Motors::lMotor.SetPercent(-40);
    sleepWithAbortCheck(3);

    Motors::lMotor.Stop();

    printLineF(3, "right motor forward...");
    Motors::rMotor.SetPercent(40);
    sleepWithAbortCheck(3);

    printLineF(4, "right motor backward...");
    Motors::rMotor.SetPercent(-40);
    sleepWithAbortCheck(3);

    Motors::rMotor.Stop();

    printLineF(5, "done.");
}

void encoderTest() {
    printLineF(1, "left motor forward...");
    Motors::lEncoder.ResetCounts();
    Motors::lMotor.SetPercent(MOTOR_POWER);
    double startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(2, "encoder reading: %i", Motors::lEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }

    printLineF(3, "left motor backward...");
    Motors::lEncoder.ResetCounts();
    Motors::lMotor.SetPercent(-MOTOR_POWER);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(4, "encoder reading: %i", Motors::lEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }
    Motors::lMotor.Stop();

    printLineF(5, "right motor forward...");
    Motors::rEncoder.ResetCounts();
    Motors::rMotor.SetPercent(MOTOR_POWER);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(6, "encoder reading: %i", Motors::rEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }

    printLineF(7, "right motor backward...");
    Motors::rEncoder.ResetCounts();
    Motors::rMotor.SetPercent(-MOTOR_POWER);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(8, "encoder reading: %i", Motors::rEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }
    Motors::rMotor.Stop();
}