#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"

#define MOTOR_POWER 40

void goToKiosk();
void motorTest();
void waitForLight();
void runSection();
void encoderTest();

FEHMotor leftMotor(FEHMotor::Motor0, 9);
FEHMotor rightMotor(FEHMotor::Motor1, 9);

DigitalEncoder leftEncoder(FEHIO::P1_0);
DigitalEncoder rightEncoder(FEHIO::P1_1);

/* DigitalInputPin leftSwitch(FEHIO::P0_0);
DigitalInputPin rightSwitch(FEHIO::P0_1); */

AnalogInputPin lightSensor(FEHIO::P0_7);

int motorPower = 25;
float time = 8.0;

int main() {
    setMotors(&leftMotor, &rightMotor);
    setEncoders(&leftEncoder, &rightEncoder);
    registerMotor(&leftMotor, 0);
    registerMotor(&rightMotor, 1);
    registerIOFunction("motorTest()", &motorTest);
    registerIOFunction("encoderTest()", &encoderTest);
    registerIOFunction("waitForLight()", &waitForLight);
    registerIOFunction("goToKiosk()", &goToKiosk);
    registerIOFunction("runSection()", &runSection);

    openIOMenu();
}


void runSection() {
    waitForLight();
    goToKiosk();
}

void goToKiosk() {
    printLineF(1, "going forward");
    drive(36, MOTOR_POWER);

    printLineF(2, "turning left");
    turn(-45, MOTOR_POWER);

    printLineF(3, "going forward");
    drive(12, MOTOR_POWER);

    printLineF(4, "turning right");
    turn(45, MOTOR_POWER);

    printLineF(5, "going forward to kiosk");
    startDriving(MOTOR_POWER);
    /* while (leftSwitch.Value() && rightSwitch.Value()) {
        abortCheck();
    } */
    Sleep(3.f);

    stopDriving();
    printLineF(6, "done");
}

void motorTest() {
    printLineF(1, "left motor forward...");
    leftMotor.SetPercent(40);
    sleepWithAbortCheck(3);
    printLineF(2, "left motor backward...");
    leftMotor.SetPercent(-40);
    sleepWithAbortCheck(3);
    leftMotor.Stop();
    printLineF(3, "right motor forward...");
    rightMotor.SetPercent(40);
    sleepWithAbortCheck(3);
    printLineF(4, "right motor backward...");
    rightMotor.SetPercent(-40);
    sleepWithAbortCheck(3);
    rightMotor.Stop();
    printLineF(5, "done.");
}

void waitForLight() {
    printWrapF(9, "Waiting for light...");

    //wait for light
    while (lightSensor.Value()>2.50); //if no light do nothing
    setDebuggerFontColor(0xFFFF00);
    printWrapF(10, "NOW AT LAST I SEEEE THE LIGHT");
    setDebuggerFontColor();
}

void encoderTest() {
    printLineF(1, "left motor forward...");
    leftEncoder.ResetCounts();
    leftMotor.SetPercent(MOTOR_POWER);
    double startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(2, "encoder reading: %i", leftEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }

    printLineF(3, "left motor backward...");
    leftEncoder.ResetCounts();
    leftMotor.SetPercent(-MOTOR_POWER);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(4, "encoder reading: %i", leftEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }
    leftMotor.Stop();

    printLineF(5, "right motor forward...");
    rightEncoder.ResetCounts();
    rightMotor.SetPercent(MOTOR_POWER);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(6, "encoder reading: %i", rightEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }

    printLineF(7, "right motor backward...");
    rightEncoder.ResetCounts();
    rightMotor.SetPercent(-MOTOR_POWER);
    startTime = TimeNow();
    while (TimeNow() < startTime + 5) {
        printLineF(8, "encoder reading: %i", rightEncoder.Counts());
        sleepWithAbortCheck(0.01f);
    }
    rightMotor.Stop();
}