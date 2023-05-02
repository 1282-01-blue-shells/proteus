#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHSD.h"
#include "FEHLCD.h"
#include "FEHServo.h"
#include "FEHRPS.h"


AnalogInputPin lightSensor(FEHIO::P0_7);

FEHServo r2d2Servo(FEHServo::Servo1);

float rampX = 30;
float rampTopY = 42;
float escapeToX = 22;
float passportLeverY = 60;
float passportLeverX = 26;
float spotA = 30;

//MORE DEFINITIONS
#define RPS_WAIT_TIME_IN_SEC 0.35
#define counts_inch 40.5 
#define counts_degree 2.48
#define pulse_time .35
#define pulse_power 15
#define PLUS 0
#define MINUS 1

//OTHER FUNCTIONS

void runCheckpoint();
void waitForLight();
//void akgoToStation();
void reinagoToStation();
void spinPassportLever();
void testLineUpAngle();
void testLineUpX();

//NEW TESTING FUNCTIONS
void check_x(float,int);
void check_y(float,int);
void check_heading(float);



int main() {
    r2d2Servo.SetMin(500);
    r2d2Servo.SetMax(2315);
    r2d2Servo.SetDegree(90);

    ProteOS::registerVariable("rampX", &rampX);
    ProteOS::registerVariable("rampTopY", &rampTopY);
    ProteOS::registerVariable("escapeToX", &escapeToX);
    ProteOS::registerVariable("passportLeverY", &passportLeverY);
    ProteOS::registerVariable("passportLeverX", &passportLeverX);

    //registering main functions
    ProteOS::registerFunction("waitForLight()", &waitForLight);
    //ProteOS::registerFunction("goToStation()", &akgoToStation);
    ProteOS::registerFunction("gotostationR()",&reinagoToStation);
    ProteOS::registerFunction("spinPassportLever()", &spinPassportLever);
    ProteOS::registerFunction("runCheckpoint()", &runCheckpoint);
    ProteOS::registerFunction("testLineUpAngle()", &testLineUpAngle);
    ProteOS::registerFunction("testLineUpX()", &testLineUpX);

    ProteOS::run();
}

//RUNNING THE CHECKPOINT
void runCheckpoint() {
    waitForLight();
    //akgoToStation();
    reinagoToStation();
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

//function to help bot navigate and also read the data needed
/*void akgoToStation() {

    //open up SD File for writing
    FEHFile *fptr = SD.FOpen("OUTPUTSD.txt","w");

    Debugger::printNextLine("headin to da ramp");

    // Starting at the light.
    // turn southwest to back up to ramp
    // line up with the center of the ramp
    //SPOT A
    check_heading(45);
    Motors::drive(6);
    Sleep(1.0);
    
    check_x(spotA, PLUS);
    check_heading(90);
    Sleep(1.0);

    SD.FPrintf(fptr, "Actual A Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
    Debugger::printNextLine("goin up da ramp");

    // SPOT B
    //turn north
    // go up ramp
    Motors::drive(20);
    check_y(rampTopY, PLUS);
    SD.FPrintf(fptr, "Actual B Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
    Debugger::printNextLine("escapin da corner");

    //SPOT C
    // turn left and escape from the corner
    check_heading(135);
    Sleep(1.0);
    Motors::drive(11);
    check_x(escapeToX, MINUS);
    SD.FPrintf(fptr, "Actual C Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());

    Debugger::printNextLine("gittin lined up");

    //SPOT D
    // back up to be in line with the passport lever
    check_heading(270);
    Sleep(1.0);
    Motors::drive(10);
    check_y(passportLeverY, PLUS);

    SD.FPrintf(fptr, "Actual D Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
    

    Debugger::printNextLine("Backing up now!");
    //SPOT E
    // turn to align back with station
    // back up
    check_heading(180);
    check_x(passportLeverX, PLUS);
    Sleep(1.0);
    SD.FPrintf(fptr, "Actual E Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
    
}*/

void reinagoToStation() {

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
    Motors::errorThresholdInches = 0.05f;
    Motors::lineUpToYCoordinate(passportLeverY);

    // turn to align back with station
    //Motors::errorThresholdDegrees = 0.25;
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

//spinning the lever
void spinPassportLever() {
    Debugger::printNextLine("TIME TO SPIN DA LEVER");
    // Rotate servo to under lever
    r2d2Servo.SetDegree(135);
    
    // Back up into lever
    Motors::drive(-2);

    /* for (int i = 0; i < 3; i++) {
        Motors::lMotor.SetPercent(-50);
        Debugger::sleep(0.25);
        Motors::lMotor.Stop();
        Debugger::sleep(0.25);
        Motors::rMotor.SetPercent(-50);
        Debugger::sleep(0.25);
        Motors::rMotor.Stop();
        Debugger::sleep(0.25);
    } */

    // Spin servo to flip lever
    rotateServoSlow(135, 0);
    Sleep(2);

    // Spin servo other way to un-flip lever
    rotateServoSlow(0, 45);
    Motors::drive(-1);
    rotateServoSlow(45, 0);

    // Drive away
    Motors::drive(2);

    // Reset servo angle
    r2d2Servo.SetDegree(90);
}

void testLineUpAngle() {
    Motors::lineUpToAngle(90);
}

void testLineUpX() {
    Motors::lineUpToXCoordinate(18);
}

//having bot move to x_coordinate using RPS
void check_x(float x_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = pulse_power;
    if (orientation == MINUS)
    {
        power = -pulse_power;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.X() >= 0 && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
    {
        if (RPS.X() > x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            Motors::pulse_forward(-power, pulse_power);
        }
        else if (RPS.X() < x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            Motors::pulse_forward(power, pulse_power);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

//having bot move to y_coordinate using RPS
void check_y(float y_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = pulse_power;
    if (orientation == MINUS)
    {
        power = -pulse_power;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.Y() >=0 && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
    {
        if (RPS.Y() > y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            Motors::pulse_forward(-power, pulse_power);
        }
        else if (RPS.Y()<y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            Motors::pulse_forward(power, pulse_power);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

float halfRoundMod(float x, float n) {
    float d = x / n;
    float m = x - d;
    if (m < 0) m += n;
    if (m > n/2) m -= n;
    return m;
}

//having bot orient itself in correct heading using RPS
void check_heading(float heading)
{

    int power = pulse_power;

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.Heading() >= 0 && (RPS.Heading() < heading - 10 || RPS.Heading() > heading + 10))
    {
        if (halfRoundMod(RPS.Heading() - heading, 360) > 0)
        {
            // Pulse the motors for a short duration in the correct direction
            Motors::pulse_counterclockwise(-power, pulse_power);
        }
        else if (halfRoundMod(RPS.Heading() - heading, 360) < 0)
        {
            // Pulse the motors for a short duration in the correct direction
            Motors::pulse_counterclockwise(power, pulse_power);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}