#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHRPS.h"
#include "FEHServo.h"
#include "FEHUtility.h"

AnalogInputPin lightSensor(FEHIO::P0_7);

FEHServo r2d2Servo(FEHServo::Servo1);
FEHServo mouthServo(FEHServo::Servo0);

float leverCorrection = 0;

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


int main() {
    mouthServo.SetMin(500);
    mouthServo.SetMax(2390);
    r2d2Servo.SetMin(500);
    r2d2Servo.SetMax(2315);
    mouthServo.SetDegree(60);
    r2d2Servo.SetDegree(90);
    
    ProteOS::registerVariable("motorPower", &Motors::maxPower);
    ProteOS::registerVariable("leverCorrection", &leverCorrection);

    ProteOS::registerFunction("runCourse()", &runCourse);

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
    Motors::errorThresholdInches = 0.7f;
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
    Motors::lineUpToXCoordinate(30);

    // turn north
    Motors::lineUpToAngle(90);

    // Motors::maxPower = 40;

    // go up ramp
    Motors::lineUpToYCoordinate(42);

    // Motors::maxPower = 20;

    precise();

    // escape corner and line up with luggage
    Motors::turn(-45);
    Motors::lineUpToXCoordinate(20);

    // move towards luggage
    Motors::lineUpToAngle(270);
    Motors::lineUpToYCoordinate(44);
}

void dropLuggage() {
    Debugger::printNextLine("BYE BYE LUGGAGE");
    mouthServo.SetDegree(125); //position lever down to drop luggage
    Debugger::sleep(1);
    mouthServo.SetDegree(60);
}

void goToLight() {

    // back up
    Motors::drive(-1);

    // turn to the side
    Motors::lineUpToAngle(315);

    // back up to be in line with the light
    Motors::lineUpToXCoordinate(11.5);

    // go to the light
    Motors::lineUpToAngle(270);
    Motors::lineUpToYCoordinate(62);
}

int getLightColor() {
    if (lightSensor.Value() < 0.3) {
        Debugger::setFontColor(0xFF0000);
        Debugger::printNextLine("Light is RED");
        Debugger::setFontColor();
        return 1;
    } else {
        Debugger::setFontColor(0x00FFFF);
        Debugger::printNextLine("Light is Blue");
        Debugger::setFontColor();
        return 0;
    }
}

void pressKioskButton() {
    int buttonNumber = getLightColor();
    // drive away from wall
    Motors::drive(6);

    Debugger::printNextLine("Button Time!!!"); // ln 6

    // go to the x coordinate of the intended button
    Motors::turn(90);
    Motors::lineUpToXCoordinate(19 + 5*buttonNumber);
    Motors::lineUpToAngle(90);

    // Move mouth down so it can hit the button
    mouthServo.SetDegree(150);
    // hit the button
    Motors::drive(4);
    // back up again
    Motors::drive(-2);
}

void goToPassportStation() {

    // Get lined up vertically
    Motors::turn(-180);
    Motors::lineUpToYCoordinate(60);

    // Go to the station
    Motors::lineUpToAngle(180);
    Motors::lineUpToXCoordinate(26);
}

void rotateR2D2ServoSlow(float start, float end) {
    float degree = start;

    // Code is different if it has to go up or down
    if (end > start) {
        // going up
        while (degree <= end) {
            r2d2Servo.SetDegree(degree);
            degree++;
            Debugger::sleep(0.01f);
        }
    } else {
        // going down
        while (degree >= end) {
            r2d2Servo.SetDegree(degree);
            degree--;
            Debugger::sleep(0.01f);
        }
    }
}

void spinPassportLever() {
    Debugger::printNextLine("ITS TIME TO SPIN DA LEVER");
    // Rotate servo to under lever
    r2d2Servo.SetDegree(135);
    
    // Back up into lever
    Motors::drive(-2);

    // Spin servo to flip lever
    rotateR2D2ServoSlow(135, 0);
    Sleep(2);

    // Spin servo other way to un-flip lever
    rotateR2D2ServoSlow(0, 45);
    // Motors::drive(-1);
    // rotateR2D2ServoSlow(45, 0);

    // Drive away
    Motors::drive(2);

    // Reset servo angle
    r2d2Servo.SetDegree(90);
}

void goBackDownTheRamp() {

    Debugger::printNextLine("Zoom!");

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

    Motors::drive(12);
    Motors::turn(-75);
    Motors::lineUpToXCoordinate(6);
    Motors::lineUpToAngle(270);

    //precise();
    Motors::lineUpToYCoordinate(20);
    Motors::lineUpToAngle(270);

    /*Motors::drive(12);
    Motors::turn(-90);
    Motors::drive(12);
    Motors::turn(-90);
    Motors::lineUpToXCoordinate(6);
    Motors::lineUpToAngle(90);

    //precise();
    Motors::lineUpToYCoordinate(20);
    Motors::lineUpToAngle(0);*/
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

    // separate function to hardcode directions for lever alignment
    if (leverNumber == 1) {
        Motors::turn(0);
        Motors::drive(2);
    } else if (leverNumber == 2) {
        Motors::turn(0);
        Motors::drive(2);
    } else if (leverNumber == 3) {
        Motors::turn(0);
        Motors::drive(2);
    }
    Motors::drive(leverCorrection);

    // Hit it down
    mouthServo.SetDegree(55);
    Motors::drive(2);
    mouthServo.SetDegree(90);
    Motors::drive(-2);

    Debugger::sleep(5);
    
    // Hit it up again
    mouthServo.SetDegree(125);
    Motors::drive(2);
    mouthServo.SetDegree(90);
    Motors::drive(-2);

    Motors::turn(-90);
}

void hitStopButton() {
    Debugger::printNextLine("Goodbye Cruel World");
    Debugger::printNextLine("HAHAH!!!");

    // leave rps dead zone
    Motors::drive(6);
    Motors::turn(45);
    Motors::drive(6);

    fast();

    // get lined up and all
    Motors::lineUpToXCoordinate(24);
    Motors::lineUpToAngle(90);
    Motors::lineUpToYCoordinate(12);

    // Hit da button
    Motors::turn(-45);
    Motors::drive(-6);
}
