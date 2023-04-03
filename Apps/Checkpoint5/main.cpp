//a program to take the suitcase and lower it into the carrier

#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHSD.h"
#include "FEHLCD.h"
#include "FEHServo.h"
#include "FEHRPS.h"


AnalogInputPin lightSensor(FEHIO::P0_7);

FEHServo r2d2Servo(FEHServo::Servo1);
FEHServo mouthServo(FEHServo::Servo0);

void waitForLight();
void gotoluggagedropoff();
void dropluggage();
void goToStopButton();
void runCheckpoint();

float rampX = 30;
float rampStartY = 12;
float rampTopY = 42;
float luggageX = 22;
float luggageY = 44;

int main() {
    mouthServo.SetMin(500);
    mouthServo.SetMax(2390);
    
    mouthServo.SetDegree(60);

    //registering main functions
    ProteOS::registerFunction("waitForLight()", &waitForLight);
    ProteOS::registerFunction("gotoluggagedropoff()", &gotoluggagedropoff);
    ProteOS::registerFunction("dropluggage()", &dropluggage);
    ProteOS::registerFunction("runCheckpoint()", &runCheckpoint);
    ProteOS::registerFunction("goToStopButton()", &goToStopButton);

    ProteOS::run();

}

void runCheckpoint() {
    waitForLight();
    gotoluggagedropoff();
    dropluggage();
    goToStopButton();
}

void waitForLight() {
    Debugger::printLine(9, "Waiting for light...");

    // wait for light
    while (lightSensor.Value()>1); // if no light do nothing


    // Change font color to yellow
    Debugger::setFontColor(0xFFFF00);

    // Print funny message
    Debugger::printWrap(10, "NOW AT LAST I SEEEE THE LIGHT");

    // Reset font color
    Debugger::setFontColor();
}

void gotoluggagedropoff() {
    // line up with the center of the ramp
    Debugger::printNextLine("headin to da ramp");

    // Motors::maxPower = 20;

    // Starting at the light.
    // turn southwest to back up to ramp
    Motors::lineUpToAngle(225);

    // line up with the center of the ramp
    Motors::lineUpToXCoordinate(rampX);

    Debugger::printNextLine("goin up da ramp");

    // turn north
    Motors::lineUpToAngle(90);

    // Motors::maxPower = 40;

    // go up ramp
    Motors::lineUpToYCoordinate(rampTopY);

    // Motors::maxPower = 20;

    Debugger::printNextLine("escapin da corner");

    // escape corner and line up with luggage
    Motors::lineUpToAngle(135);
    Motors::lineUpToXCoordinate(luggageX);

    // move towards luggage
    Motors::lineUpToAngle(270);
    Motors::lineUpToYCoordinate(luggageY);

}

void dropluggage(){
    mouthServo.SetDegree(125); //position lever down to drop luggage
    Debugger::sleep(2.0f);
    mouthServo.SetDegree(60);
}

void goToStopButton() {
    // go to top of ramp
    Motors::lineUpToAngle(180);
    Motors::lineUpToXCoordinate(rampX);

    // go down ramp
    Motors::lineUpToAngle(90);
    Motors::lineUpToYCoordinate(rampStartY);
    
    // line up with button
    Motors::lineUpToAngle(180);
    Motors::lineUpToXCoordinate(24);
    Motors::lineUpToAngle(135);

    // push button
    Motors::drive(-10);

}
