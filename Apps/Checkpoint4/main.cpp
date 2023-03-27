#include "proteos.hpp"
#include "navigation.hpp"


#include "FEHLCD.h"
#include "FEHServo.h"
#include "FEHRPS.h"


AnalogInputPin lightSensor(FEHIO::P0_7);

FEHServo r2d2Servo(FEHServo::Servo1);

float rampX = 30;
float rampTopY = 40;
float escapeToX = 18;
float passportLeverY = 59;
float passportLeverX = 26;


void runCheckpoint();
void waitForLight();
void goToStation();
void spinPassportLever();


int main() {
    r2d2Servo.SetMin(500);
    r2d2Servo.SetMax(2315);
    r2d2Servo.SetDegree(90);

    ProteOS::registerVariable("rampX", &rampX);
    ProteOS::registerVariable("rampTopY", &rampTopY);
    ProteOS::registerVariable("escapeToX", &escapeToX);
    ProteOS::registerVariable("passportLeverY", &passportLeverY);
    ProteOS::registerVariable("passportLeverX", &passportLeverX);

    ProteOS::registerFunction("waitForLight()", &waitForLight);
    ProteOS::registerFunction("goToStation()", &goToStation);
    ProteOS::registerFunction("spinPassportLever()", &spinPassportLever);
    ProteOS::registerFunction("runCheckpoint()", &runCheckpoint);

    ProteOS::run();
}

void runCheckpoint() {
    waitForLight();
    goToStation();
    spinPassportLever();
}

void waitForLight() {
    Debugger::printLine(9, "Waiting for light...");

    // wait for light
    // if light low (voltage high) then wait
    while (lightSensor.Value() > 1);


    // Change font color to yellow
    Debugger::setFontColor(0xFFFF00);

    // Print funny message
    Debugger::printWrap(10, "NOW AT LAST I SEEEE THE LIGHT");

    // Reset font color
    Debugger::setFontColor();
}

void goToStation() {

    Debugger::printNextLine("headin to da ramp");

    // Starting at the light.
    // turn southwest to back up to ramp
    Motors::lineUpToAngle(225);

    // line up with the center of the ramp
    Motors::lineUpToXCoordinate(rampX);

    Debugger::printNextLine("goin up da ramp");

    // turn north
    Motors::lineUpToAngle(90);

    // go up ramp
    Motors::lineUpToYCoordinate(rampTopY);

    Debugger::printNextLine("escapin da corner");

    // turn left and escape from the corner
    Motors::lineUpToAngle(135);
    Motors::lineUpToXCoordinate(escapeToX);

    Debugger::printNextLine("gittin lined up");

    // back up to be in line with the passport lever
    Motors::lineUpToAngle(270);
    Motors::lineUpToYCoordinate(passportLeverY);

    // turn to align back with station
    Motors::lineUpToAngle(180);

    // back up
    Motors::lineUpToXCoordinate(passportLeverX);

}

void rotateServoSlow(float start, float end) {
    float degree = start;

    // Code is different if it has to go up or down
    if (end > start) {
        // going up
        while (degree <= end) {
            r2d2Servo.SetDegree(degree);
            degree++;
            Debugger::sleep(0.02f);
        }
    } else {
        // going down
        while (degree >= end) {
            r2d2Servo.SetDegree(degree);
            degree--;
            Debugger::sleep(0.02f);
        }
    }
}

void spinPassportLever() {
    Debugger::printNextLine("TIME TO SPIN DA LEVER");
    // Rotate servo to under lever
    r2d2Servo.SetDegree(45);
    
    // Back up into lever
    Motors::drive(-2);

    // Spin servo to flip lever
    rotateServoSlow(45, 170);
    Sleep(2);

    // Spin servo other way to un-flip lever
    rotateServoSlow(170, 45);

    // Drive away
    Motors::drive(4);

    // Reset servo angle
    r2d2Servo.SetDegree(90);
}