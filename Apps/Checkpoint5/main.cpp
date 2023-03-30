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
void runCheckpoint();

float rampstart = 30;
float ramptop = 42;
float bluggage = 22;
float atluggage = 40;

int main (){
    mouthServo.SetMin(500);
    mouthServo.SetMax(2390);
    
    mouthServo.SetDegree(90);

    //registering main functions
    ProteOS::registerFunction("waitForLight()", &waitForLight);
    ProteOS::registerFunction("gotoluggagedropoff()",&gotoluggagedropoff);
    ProteOS::registerFunction("dropluggage()",&dropluggage);
    ProteOS::registerFunction("runcheckpoint()",&runCheckpoint);

    ProteOS::run();

}

void runCheckpoint() {
    waitForLight();
    gotoluggagedropoff();
    dropluggage();

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

void gotoluggagedropoff () {
    // line up with the center of the ramp
    Debugger::printNextLine("headin to da ramp");

    Motors::maxPower = 20;

    // Starting at the light.
    // turn southwest to back up to ramp
    Motors::lineUpToAngle(225);

    // line up with the center of the ramp
    Motors::lineUpToXCoordinate(rampstart);

    Debugger::printNextLine("goin up da ramp");

    // turn north
    Motors::lineUpToAngle(90);

    Motors::maxPower = 40;

    // go up ramp
    Motors::lineUpToYCoordinate(ramptop);

    Motors::maxPower = 20;

    //before lining up with luggage carrier
    Motors::lineUpToAngle(135);
    Motors::lineUpToXCoordinate(bluggage);

    //moving towards luggage carrier
    Motors::lineUpToAngle(270);
    Motors::lineUpToYCoordinate(atluggage);

}

void dropluggage(){
    mouthServo.SetDegree(125); //position lever down to drop luggage
    Debugger::sleep(2.0f);
    mouthServo.SetDegree(90);
}

