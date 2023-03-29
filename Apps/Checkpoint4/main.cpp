#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHSD.h"
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
void goToStation();
void spinPassportLever();

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
    ProteOS::registerFunction("goToStation()", &goToStation);
    ProteOS::registerFunction("spinPassportLever()", &spinPassportLever);
    ProteOS::registerFunction("runCheckpoint()", &runCheckpoint);

    ProteOS::run();
}

//RUNNING THE CHECKPOINT
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

//function to help bot navigate and also read the data needed
void goToStation() {

    float Ax,Ay,Bx,By,Cx,Cy,Dx,Dy,Ex,Ey;
    float Ah,Bh,Ch,Dh,Eh;

    //open up SD File for writing
    FEHFile *fptr = SD.FOpen("OUTPUTSD.txt","w");

    Debugger::printNextLine("headin to da ramp");

    // Starting at the light.
    // turn southwest to back up to ramp
    // line up with the center of the ramp
    //SPOT A
    check_x(rampX, PLUS);
    check_heading(225);
    Sleep(1.0);

    SD.FPrintf(fptr, "Actual A Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
    Debugger::printNextLine("goin up da ramp");

    // SPOT B
    //turn north
    // go up ramp
    check_y(rampTopY, PLUS);
    check_heading(90);
    Sleep(1.0);

    SD.FPrintf(fptr, "Actual B Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
    Debugger::printNextLine("escapin da corner");

    //SPOT C
    // turn left and escape from the corner
    check_x(escapeToX, MINUS);
    check_heading(135);
    Sleep(1.0);
    SD.FPrintf(fptr, "Actual C Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());

    Debugger::printNextLine("gittin lined up");

    //SPOT D
    // back up to be in line with the passport lever
    check_y(passportLeverY, PLUS);
    check_heading(270);
    Sleep(1.0);

    SD.FPrintf(fptr, "Actual D Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
    
    //SPOT E
    // turn to align back with station
    // back up
    check_x(passportLeverX, PLUS);
    check_heading(180);
    Sleep(1.0);
    SD.FPrintf(fptr, "Actual E Position: %f %f %f \n", RPS.X(), RPS.Y(),RPS.Heading());
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