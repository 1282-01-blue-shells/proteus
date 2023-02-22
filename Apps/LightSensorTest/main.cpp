#include <proteos.hpp>

#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <FEHUtility.h>

//WHERE ALL THE DEFINITIONS ARE
#define MOTOR_POWER_MED 15.f
#define MOTOR_STOP 0.0f

//DECLARING INPUTS
AnalogInputPin cds(FEHIO::P0_0); //configure Cds cell as an analog input
FEHMotor motorLeft(FEHMotor::Motor0,9.); //declare two motors (might change once move to actual bot)
FEHMotor motorRight(FEHMotor::Motor1,9.);

//DECLARING FUNCTIONS
void StartAtLight();
void TraveltoKiosk();

int main() {
    registerIOFunction("StartAtLight()", &StartAtLight);
    registerIOFunction("TraveltoKiosk()", &TraveltoKiosk);

    openIOMenu(); // "runs the os" opens the interface to run functions
}

//function to test whether robot responds to starting light
void StartAtLight() {
    //start at back wall and wait for input
    breakpoint();
    printWrapF(1, "Received input, waiting for light");

    //wait for light
    while (cds.Value()>2.50); //if no light do nothing
    printWrapF(3, "NOW AT LAST I SEEEE THE LIGHT");
    void TraveltoKiosk();
}

//function to allow robot to travel to kiosk (right now testing if react to light turning on)
void TraveltoKiosk(){
    motorLeft.SetPercent(MOTOR_POWER_MED);
    motorRight.SetPercent(MOTOR_POWER_MED);
    Sleep(2.0);
    motorLeft.SetPercent(MOTOR_STOP);
    motorRight.SetPercent(MOTOR_STOP);
    
}
/*
AnalogInputPin cds(FEHIO::P0_0);  //Configure CdS cell as an analog input
FEHMotor motorLeft(FEHMotor::Motor0, 9.);
FEHMotor motorRight(FEHMotor::Motor1,9.);
FEHServo servo(FEHServo::Servo7); //declare servo arm
DigitalInputPin switchFrontLeft(FEHIO::P0_0); //Configuring each of the microswitches as a digital input
DigitalInputPin switchFrontRight(FEHIO::P1_0);
DigitalInputPin switchBackLeft(FEHIO::P2_0);
DigitalInputPin switchBackRight(FEHIO::P3_0);
void partOne();
void partTwo();
void partThree();
void driveForward();
void driveBackward();
void driveBackwardLeft();
void driveBackwardRight();
void stopMotors();
void waitUntilHittingWall();
void testingcdsvalues() {
    while (true) {
        LCD.Clear(BLACK);
        LCD.WriteLine(cds.Value()); //printing the output of the CDS cell to the LDS Screen
        Sleep(0.1);
    }
}
void partThree() {
    LCD.Clear(BLACK);
    LCD.WriteLine("Waiting for input..."); //start at back wall and wait for input
    float x, y;
    // wait until LCD touch
    while (!LCD.Touch(&x, &y));
    while (LCD.Touch(&x, &y));
    LCD.Clear();
    LCD.WriteLine("Input received, starting");
    // 1
    driveForward();
    waitUntilHittingWall(); //drive forward until reach wall
    LCD.Clear();
    LCD.WriteLine("Oh noes X(");
    // 2
    driveBackwardLeft(); //turn back 90 degrees so that it aligns with the left wall
    waitUntilHittingWall();
    // 3
    driveForward(); //move forward so that it goes face forward into the right wall
    waitUntilHittingWall();
    // 4
    driveBackwardRight(); //turn backwards 90 degrees so that bot goes back into first divider
    waitUntilHittingWall();
    // 5
    driveForward(); //bot moves forward to front wall
    waitUntilHittingWall();
    LCD.Clear();
    LCD.WriteLine("<3");
    LCD.Clear();
    LCD.WriteLine(":3");
    LCD.Clear();
    LCD.WriteLine("yippee");
}
void driveForward() {
    motorLeft.SetPercent(MOTOR_POWER_FAST);
    motorRight.SetPercent(MOTOR_POWER_FAST);
}
void driveBackward() {
    motorLeft.SetPercent(-MOTOR_POWER_FAST);
    motorRight.SetPercent(-MOTOR_POWER_FAST);
}
void driveBackwardLeft() {
    motorLeft.SetPercent(-MOTOR_POWER_SLOW);
    motorRight.SetPercent(-MOTOR_POWER_FAST);
    Sleep(3.0);
    motorLeft.SetPercent(-MOTOR_POWER_MED);
    motorRight.SetPercent(-MOTOR_POWER_MED);
}
void driveBackwardRight() {
    motorLeft.SetPercent(-MOTOR_POWER_FAST);
    motorRight.SetPercent(-MOTOR_POWER_SLOW);
    Sleep(3.0);
    motorLeft.SetPercent(-MOTOR_POWER_MED);
    motorRight.SetPercent(-MOTOR_POWER_MED);
}
void stopMotors() {
    motorLeft.Stop();
    motorRight.Stop();
}
void waitUntilHittingWall() {
    Sleep(0.5);
    while (true) {
        bool frontLeft = switchFrontLeft.Value();
        bool frontRight = switchFrontRight.Value();
        bool backLeft = switchBackLeft.Value();
        bool backRight = switchBackRight.Value();
        if ((!frontLeft && !frontRight) || (!backLeft && !backRight)) {
            Sleep(0.1);
            stopMotors();
            return;
        }
    }
}*/