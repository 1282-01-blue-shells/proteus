#include "proteos.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"

void driveForward();

FEHMotor leftMotor(FEHMotor::Motor0, 9);
FEHMotor rightMotor(FEHMotor::Motor1, 9);

int motorPower = 25;
float time = 8.0;

int main() {
    registerMotor(&leftMotor, 0);
    registerMotor(&rightMotor, 1);
    registerIOVariable("motorPower", &motorPower);
    registerIOVariable("time", &time);
    registerIOFunction("driveForward()", &driveForward);

    openIOMenu();
}

void driveForward() {
    leftMotor.SetPercent(motorPower);
    rightMotor.SetPercent(motorPower);
    sleepWithAbortCheck(time);
    leftMotor.Stop();
    rightMotor.Stop();
}