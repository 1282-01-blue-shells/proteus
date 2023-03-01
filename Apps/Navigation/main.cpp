#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"

#define MOTOR_POWER 40

void goToKiosk();

FEHMotor leftMotor(FEHMotor::Motor0, 9);
FEHMotor rightMotor(FEHMotor::Motor1, 9);

DigitalEncoder leftEncoder(FEHIO::P1_0);
DigitalEncoder rightEncoder(FEHIO::P1_1);

DigitalInputPin leftSwitch(FEHIO::P0_0);
DigitalInputPin rightSwitch(FEHIO::P0_1);

int motorPower = 25;
float time = 8.0;

int main() {
    setMotors(&leftMotor, &rightMotor);
    setEncoders(&leftEncoder, &rightEncoder);
    registerMotor(&leftMotor, 0);
    registerMotor(&rightMotor, 1);
    registerIOFunction("goToKiosk()", &goToKiosk);

    openIOMenu();
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
    while (leftSwitch.Value() && rightSwitch.Value()) {
        abortCheck();
    }

    stopDriving();
    printLineF(6, "done");
}
