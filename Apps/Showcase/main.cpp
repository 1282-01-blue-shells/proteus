#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHRPS.h"
#include "FEHServo.h"
#include "FEHUtility.h"

#include <cmath>

AnalogInputPin lightSensor(FEHIO::P0_7);

FEHServo r2d2Servo(FEHServo::Servo1);
FEHServo mouthServo(FEHServo::Servo0);

static float leverCorrection = 1.5;
//static float leverAngles[3] = { -30.f, -10.f, 20.f };
static bool doPassportLeverCorrection = true;
static float initiallever = 10;
static float overshoot = 2;
static float distancetolever = -2.5;
static float otherLeverCorrection = -3;
static bool doWillThing = false;

void runCourse();
void waitForLight();
void goToLuggageDropoff();
void dropLuggage();
void goToLight();
void pressKioskButton();
void goToPassportStation();
void spinPassportLever();
void goBackDownTheRamp();
void goToLevers();
void flipLever();
void hitStopButton();

int getLightColor();

void OLD_runCourse();


int main() {
    mouthServo.SetMin(500);
    mouthServo.SetMax(2390);
    r2d2Servo.SetMin(500);
    r2d2Servo.SetMax(2315);
    mouthServo.SetDegree(60);
    r2d2Servo.SetDegree(90);
    
    ProteOS::registerVariable("motorPower", &Motors::maxPower);
    ProteOS::registerVariable("leverCorrection", &leverCorrection);
    /* ProteOS::registerVariable("leverAngle0", &leverAngles[0]);
    ProteOS::registerVariable("leverAngle1", &leverAngles[1]);
    ProteOS::registerVariable("leverAngle2", &leverAngles[2]); */
    /* ProteOS::registerVariable("leverDist0", &leverDists[0]);
    ProteOS::registerVariable("leverDist1", &leverDists[1]);
    ProteOS::registerVariable("leverDist2", &leverDists[2]); */
    ProteOS::registerVariable("doPassportLeverCorrection", &doPassportLeverCorrection);
    ProteOS::registerVariable("otherLeverCorrection", &otherLeverCorrection);
    ProteOS::registerVariable("doWillThing", &doWillThing);

    ProteOS::registerFunction("runCourse()", &runCourse);
    ProteOS::registerFunction("OLD_runCourse()", &OLD_runCourse);

    ProteOS::run();
}

void runCourse() {
    waitForLight();
    goToLuggageDropoff();
    dropLuggage();
    goToLight();
    pressKioskButton();
    goToPassportStation();
    spinPassportLever();
    goBackDownTheRamp();
    goToLevers();
    flipLever();
    hitStopButton();
}

void precise() {
    Motors::errorThresholdDegrees = 0.5f;
    Motors::errorThresholdInches = 0.1f;
}

void fast() {
    Motors::errorThresholdDegrees = 1.5;
    Motors::errorThresholdInches = 0.5f;
}

void waitForLight() {
    Debugger::printLine(0, "Waiting for light...");

    float startTime = TimeNow();

    // wait for light
    while (lightSensor.Value()>1) { // if no light do nothing
        Debugger::abortCheck();
        if (TimeNow() > startTime + 30) break;
    }


    // Change font color to yellow
    Debugger::setFontColor(0xFFFF00);

    // Print funny message
    Debugger::printWrap(1, "NOW AT LAST I SEEEE THE LIGHT");

    // Reset font color
    Debugger::setFontColor();
}

void goToLuggageDropoff() {

    Debugger::printNextLine("goin up da ramp");

    fast();

    // Starting at the light.
    // turn southwest to back up to ramp
    // Motors::lineUpToAngle(225);

    // line up with the center of the ramp
    //Motors::lineUpToXCoordinate(30);
    Motors::lineUpToXCoordinateMaintainHeading(30, 225);

    // turn north
    /* Motors::lineUpToAngle(90);

tryRamp:
    Motors::drive(24);
    if (RPS.Heading() < 80 || RPS.Heading() > 100) {
        Motors::drive(-12);
        goto tryRamp;
    } */
    

    // go up ramp
    //Motors::lineUpToYCoordinate(42);
    Motors::lineUpToYCoordinateMaintainHeading(42, 90);

    //precise();

    // escape corner and line up with luggage
    //Motors::turn(-60);
    //Motors::lineUpToXCoordinate(20);
    Motors::lineUpToXCoordinateMaintainHeading(20, 150);

    // move towards luggage
    //Motors::lineUpToAngle(270);
    //Motors::lineUpToYCoordinate(45);
    Motors::lineUpToYCoordinateMaintainHeading(45, 270);
}

void dropLuggage() {
    Debugger::printNextLine("BYE BYE LUGGAGE");
    mouthServo.SetDegree(125); //position lever down to drop luggage
    Debugger::sleep(0.5f);
    mouthServo.SetDegree(60);
}

void goToLight() {

    // back up
    Motors::drive(-3);

    precise();

    // turn to the side
    //Motors::lineUpToAngle(315);

    // back up to be in line with the light
    //Motors::lineUpToXCoordinate(11.5f);
    Motors::lineUpToXCoordinateMaintainHeading(11.5f, 315);

    // go to the light
    //Motors::lineUpToAngle(270);
    //Motors::lineUpToYCoordinate(64);
    Motors::lineUpToYCoordinateMaintainHeading(64, 270);
}

int getLightColor() {
    // 0 means blue, 1 means red
    int color = 0;

    float startTime = TimeNow();
    Motors::lMotor.SetPercent(20);
    Motors::rMotor.SetPercent(20);
    while (TimeNow() < startTime + 1) {
        if (lightSensor.Value() < 0.4) {
            color = 1;
            break;
        }
    }
    Motors::stop();

    if (color == 1) {
        Debugger::setFontColor(0xFF0000);
        Debugger::printNextLine("Light is RED");
        Debugger::setFontColor();
    } else {
        Debugger::setFontColor(0x00FFFF);
        Debugger::printNextLine("Light is Blue");
        Debugger::setFontColor();
    }
    return color;
}

void pressKioskButton() {
    int buttonNumber = getLightColor();
    fast();
    // drive away from wall
    //Motors::drive(6);
    Motors::lineUpToYCoordinate(56);

    Debugger::printNextLine("Button Time!!!"); // ln 6

    // go to the x coordinate of the intended button
    Motors::turn(90);
    Motors::lineUpToXCoordinate(19 + 4*buttonNumber);
    Motors::lineUpToAngle(90);

    // Move mouth down so it can hit the button
    mouthServo.SetDegree(150);
    // hit the button
    Motors::drive(4);
    // back up again
    Motors::drive(-2);
}

void goToPassportStation() {

    precise();

    // Get lined up vertically
    Motors::turn(-180);
    //Motors::lineUpToYCoordinate(60);
    Motors::lineUpToYCoordinateMaintainHeading(60.5, 270);

    // Go to the station
    //Motors::lineUpToAngle(180);
    //Motors::lineUpToXCoordinate(24);
    Motors::lineUpToXCoordinateMaintainHeading(24, 180);
}

void rotateR2D2ServoSlow(float start, float end) {
    float degree = start;
    
    // Code is different if it has to go up or down
    if (end > start) {
        // going up
        while (degree <= end) {
            r2d2Servo.SetDegree(degree);
            degree++;
            Debugger::sleep(0.003f);
        }
    } else {
        // going down
        while (degree >= end) {
            r2d2Servo.SetDegree(degree);
            degree--;
            Debugger::sleep(0.003f);
        }
    }
}

void spinPassportLever() {
    Debugger::printNextLine("ITS TIME TO SPIN DA LEVER");

    if (doPassportLeverCorrection) {
        Motors::lineUpToAngle(180 - (RPS.Y() - 60) / 6 * 180 / 3.14f);
    }
    r2d2Servo.SetDegree(135);
    Motors::drive(-4);
    Motors::turn(10);
    Motors::drive(-1);
    Motors::turn(-20);
    Motors::drive(-1);
    
    // Rotate servo to under lever
    r2d2Servo.SetDegree(135);
    
    // Back up into lever
    Motors::drive(-2);

    // Spin servo to flip lever
    rotateR2D2ServoSlow(135, 0);
    Sleep(2);

    // Spin servo other way to un-flip lever
    rotateR2D2ServoSlow(0, 90);
    // Motors::drive(-1);
    // rotateR2D2ServoSlow(45, 0);

    // Drive away
    Motors::drive(2);

    // Reset servo angle
    r2d2Servo.SetDegree(90);
}

void goBackDownTheRamp() {

    Debugger::printNextLine("I'm outta here");

    fast();

    // leave the passport station
    /* Motors::lineUpToXCoordinate(18);

    // go down to somewhere in the middle of the top section
    Motors::lineUpToAngle(270);
    Motors::lineUpToYCoordinate(52);

    // go to the top of the ramp
    Motors::lineUpToAngle(135);
    Motors::lineUpToXCoordinate(30);

    // down the ramp
    Motors::lineUpToAngle(90);
    Motors::lineUpToYCoordinate(12); */

    /* Motors::drive(12);
    Motors::turn(-75);
    Motors::lineUpToXCoordinate(6);
    Motors::lineUpToAngle(270);
    Motors::drive(6); */

    //precise();
    //Motors::lineUpToYCoordinate(20);
    //Motors::lineUpToAngle(270);

    Motors::drive(12);
    Motors::turn(-90);
    Motors::drive(12);
    Motors::turn(-90);
    Motors::lineUpToXCoordinate(6);
    Motors::lineUpToAngle(270);

    //precise();
    //Motors::lineUpToYCoordinate(20);
    //Motors::drive(32);

    Motors::start();
    Debugger::sleep(4);
    Motors::stop();

    Motors::drive(otherLeverCorrection);
    //Motors::lineUpToAngle(0);
    Motors::turn(-90);
}

void goToLevers() {

    // precise();

    /* Motors::lineUpToAngle(135);
    Motors::lineUpToYCoordinate(23);

    // Escape the ramp
    Motors::lineUpToAngle(180);
    Motors::drive(4);

    // turn around so it wont't hit the wall
    Motors::turn (-180);
    Motors::lineUpToAngle(0); */
}

void flipLever() {
    int leverNumber = RPS.GetCorrectLever();
    Debugger::printNextLine("It's leverin time (%i)", leverNumber);
    // Go to the intended lever
    /* Motors::lineUpToXCoordinate(12 - 3*leverNumber);
    // Turn to face it
    Motors::turn(90); */

    // Aligning with specific lever
    /* Motors::lineUpToXCoordinate(12 - 3*leverNumber + 2);
    Motors::turn(45);
    Motors::drive(2.82);
    Motors::turn(45);*/

    /* if (doWillThing) {
        const float angle = leverAngles[leverNumber];
        Motors::turn(angle);
        Motors::drive(4.f/std::tan(std::abs(angle)) + leverCorrection);
    } else { */
        Motors::lineUpToXCoordinate(12.5 - 3.5*leverNumber + 2);
        Motors::turn(45);
        Motors::drive(-2.82);
        Motors::turn(55);
        
        Motors::drive(leverCorrection);
    // }

    //BACK UP CODE
    /*
    Motors::turn(-90); //code for robot to turn to face so back is to wall
    Motors::drive(-8); //robot will back into wall
    Debugger::printNextLine("Going to lever %i", leverNumber);
    Motors::drive(initiallever + overshoot - 3.5f * leverNumber); //robot goes to correct lever ideally

    // turn partway to the lever
    Motors::turn(45);
    Motors::drive(-1.41*overshoot);
    Motors::turn(45); // rest of the way
    Motors::drive(overshoot + distToLever);
    */

    // Hit it down
    mouthServo.SetDegree(45);
    Motors::drive(2);
    mouthServo.SetDegree(80);
    Motors::drive(-2);

    Debugger::sleep(5);
    
    // Hit it up again
    mouthServo.SetDegree(115);
    Motors::drive(2);
    mouthServo.SetDegree(80);
    Motors::drive(-2);

    Motors::turn(-90);
}

void hitStopButton() {
    Debugger::printNextLine("Goodbye World");

    // leave rps dead zone
    Motors::drive(8);
    Motors::turn(45);
    Motors::drive(6);

    fast();

    Motors::lineUpToAngle(315);
    float xOff = 36 - RPS.X();
    float yOff = -RPS.Y();
    float angle = atan(yOff / xOff);
    Motors::lineUpToAngle(angle * 180 / 3.14);
    Motors::drive(12);

    // if that failed:
    // back up a bit
    Motors::drive(-8);

    // get lined up and all
    //Motors::lineUpToXCoordinate(24);
    Motors::lineUpToXCoordinateMaintainHeading(24, 315);
    //Motors::lineUpToAngle(90);
    //Motors::lineUpToYCoordinate(14);
    Motors::lineUpToYCoordinateMaintainHeading(14, 90);

    // Hit da button
    Motors::turn(-45);
    Motors::drive(-12);
}
