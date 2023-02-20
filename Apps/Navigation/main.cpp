#include "proteos.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"

void driveForward();

FEHMotor leftMotor(FEHMotor::Motor0, 9);
FEHMotor rightMotor(FEHMotor::Motor1, 9);

int motorPower = 25;
float time = 8.0;

int main() {
    registerIOVariable("motorPower", &motorPower);
    registerIOVariable("time", &time);
    registerIOFunction("driveForward()", &driveForward);

    openIOMenu();
}

void driveForward() {
    leftMotor.SetPercent(motorPower);
    rightMotor.SetPercent(motorPower);
    Sleep(time);
    leftMotor.Stop();
    rightMotor.Stop();
}