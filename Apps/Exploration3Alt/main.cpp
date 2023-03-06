#include "proteos.hpp"
#include "navigation.hpp"

#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHSD.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

// Number of points of interest (i.e. A, B, C, D)
#define NUM_POINTS_OF_INTEREST 4

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

// Shaft encoding counts for CrayolaBots
#define COUNTS_PER_INCH 40.5
#define COUNTS_PER_DEGREE 2.48

/* Defines for how long each pulse should be and at what motor power. 
These value will normally be small, but you should play around with the values to find what works best */
#define PULSE_TIME .35
#define PULSE_POWER 10

// Define for the motor power while driving (not pulsing)
#define POWER 20

#define HEADING_TOLERANCE 2

/* Direction along axis which robot is traveling
Examples:
	- if robot is traveling to the upper level, that is a PLUS as the y-coordinate is increasing
	- if robot is traveling to the lower level, that is a MINUS as the y-coordinate is decreasing
*/
#define PLUS 0
#define MINUS 1

// Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P2_0);
DigitalEncoder left_encoder(FEHIO::P2_1);
FEHMotor right_motor(FEHMotor::Motor1, 9.0);
FEHMotor left_motor(FEHMotor::Motor0, 9.0);

void setPOIs()
{
    // Declare variables
    float touch_x, touch_y;
    char points[NUM_POINTS_OF_INTEREST] = {'A', 'B', 'C', 'D'};

    // Open SD file for writing
    FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "w");

    Sleep(100);
    LCD.Clear();

    // Wait for touchscreen to be pressed and released
    LCD.WriteLine("Press Screen to Record");
    while (!LCD.Touch(&touch_x, &touch_y));
    while (LCD.Touch(&touch_x, &touch_y));

    LCD.ClearBuffer();

    // Clear screen
    Sleep(100); // wait for 100ms to avoid updating the screen too quickly
    LCD.Clear();

    // Write initial screen info
    LCD.WriteRC("X Position:", 11, 0);
    LCD.WriteRC("Y Position:", 12, 0);
    LCD.WriteRC("   Heading:", 13, 0);

    // Step through each path point to record position and heading
    for (int n = 0; n < NUM_POINTS_OF_INTEREST; n++)
    {
        // Write point letter
        LCD.WriteRC("Touch to set point ", 9, 0);
        LCD.WriteRC(points[n], 9, 20);

        // Wait for touchscreen to be pressed and display RPS data
        while (!LCD.Touch(&touch_x, &touch_y))
        {
            LCD.WriteRC(RPS.X(), 11, 12);       // update the x coordinate
            LCD.WriteRC(RPS.Y(), 12, 12);       // update the y coordinate
            LCD.WriteRC(RPS.Heading(), 13, 12); // update the heading

            Sleep(100); // wait for 100ms to avoid updating the screen too quickly
        }
        while (LCD.Touch(&touch_x, &touch_y));
        LCD.ClearBuffer();

        // Print RPS data for this path point to file
        SD.FPrintf(fptr, "%f %f\n", RPS.X(), RPS.Y());
    }

    // Close SD file
    SD.FClose(fptr);
    LCD.Clear();
}

/*
 * Pulse forward a short distance using time
 */
void pulse_forward(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Pulse counterclockwise a short distance using time
 */
void pulse_counterclockwise(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Move forward using shaft encoders where percent is the motor percent and counts is the distance to travel
 */
void move_forward(int percent, int counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Turn counterclockwise using shaft encoders where percent is the motor percent and counts is the distance to travel
 */
void turn_counterclockwise(int percent, int counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Use RPS to move to the desired x_coordinate based on the orientation of the QR code
 */
void check_x(float x_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.X() >= 0 && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
    {
        if (RPS.X() > x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
        }
        else if (RPS.X() < x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/*
 * Use RPS to move to the desired y_coordinate based on the orientation of the QR code
 */
void check_y(float y_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.Y() >=0 && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
    {
        if (RPS.Y() > y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
        }
        else if (RPS.Y()<y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
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


/*
 * Use RPS to move to the desired heading
 */
void check_heading(float heading)
{
    // You will need to fill out this one yourself and take into account
    // checking for proper RPS data and the edge conditions
    //(when you want the robot to go to 0 degrees or close to 0 degrees)

    /*
        SUGGESTED ALGORITHM:
        1. Check the current orientation of the QR code and the desired orientation of the QR code
        2. Check if the robot is within the desired threshold for the heading based on the orientation
        3. Pulse in the correct direction based on the orientation
    */

    int power = PULSE_POWER;

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.Heading() >= 0 && (RPS.Heading() < heading - 10 || RPS.Heading() > heading + 10))
    {
        if (halfRoundMod(RPS.Heading() - heading, 360) > 0)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(-power, PULSE_TIME);
        }
        else if (halfRoundMod(RPS.Heading() - heading, 360) < 0)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

void goToPOIs() {
    float touch_x, touch_y;

    float A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;
    float A_heading, B_heading, C_heading, D_heading;
    int B_C_counts, C_D_counts, turn_90_counts, turn_180_counts;

    LCD.Clear();
    LCD.WriteLine("Press Screen To Start Run");
    while (!LCD.Touch(&touch_x, &touch_y));
    while (LCD.Touch(&touch_x, &touch_y));

    //READ SD CARD FOR LOGGED X AND Y DATA POINTS
    FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "r");
    SD.FScanf(fptr, "%f%f", &A_x, &A_y);
    SD.FScanf(fptr, "%f%f", &B_x, &B_y);
    SD.FScanf(fptr, "%f%f", &C_x, &C_y);
    SD.FScanf(fptr, "%f%f", &D_x, &D_y);
    SD.FClose(fptr);
    FEHFile *ofptr = SD.FOpen("OutputSD.txt","w");
    SD.FPrintf(ofptr, "INT: %d, FLOAT: %f",A_x,A_y);
    SD.FPrintf(ofptr, "INT: %d, FLOAT: %f",B_x,B_y);
    SD.FPrintf(ofptr, "INT: %d, FLOAT: %f",C_x,C_y);
    SD.FPrintf(ofptr, "INT: %d, FLOAT: %f",D_x,D_y);
    SD.FClose(ofptr);

    //SET THE HEADING DEGREES AND COUNTS VALUES
    A_heading = 180;
    B_heading = 270;
    C_heading = 90;
    D_heading = 0;

    B_C_counts = COUNTS_PER_INCH * abs(C_x - B_x);
    C_D_counts = COUNTS_PER_INCH * abs(D_x - C_x);

    turn_90_counts = COUNTS_PER_DEGREE * 90;
    turn_180_counts = turn_90_counts * 2;

    
    // Open file pointer for writing
    fptr = SD.FOpen("RESULTS.txt", "w");

    // A --> B
    check_y(B_y, PLUS);
    check_heading(B_heading);
    Sleep(1.0);
    
    //WRITE EXPECTED AND ACTUAL POSITION INFORMATION TO SD CARD
    SD.FPrintf(fptr, "Expected B Position: %f %f %f\n", B_x, B_y, B_heading);
    SD.FPrintf(fptr, "Actual B Position:   %f %f %f\n\n", RPS.X(), RPS.Y(), RPS.Heading());

    //Log

    // B --> C
    move_forward(POWER, B_C_counts);
    check_x(C_x, MINUS);
    turn_counterclockwise(POWER,  turn_180_counts);
    check_heading(C_heading);
    Sleep(1.0);

    //WRITE EXPECTED AND ACTUAL POSITION INFORMATION TO SD CARD
    SD.FPrintf(fptr, "Expected B Position: %f %f %f\n", C_x, C_y, C_heading);
    SD.FPrintf(fptr, "Actual B Position:   %f %f %f\n\n", RPS.X(), RPS.Y(), RPS.Heading());

    // C --> D
    move_forward(POWER, C_D_counts);
    check_x(D_x, PLUS);
    turn_counterclockwise(-POWER, turn_90_counts);
    check_heading(D_heading);
    check_y(D_y, MINUS);
    Sleep(1.0);

    //WRITE EXPECTED AND ACTUAL POSITION INFORMATION TO SD CARD
    SD.FPrintf(fptr, "Expected B Position: %f %f %f\n", D_x, D_y, D_heading);
    SD.FPrintf(fptr, "Actual B Position:   %f %f %f\n\n", RPS.X(), RPS.Y(), RPS.Heading());
    
    // Close file pointer
    SD.FClose(fptr);
}

void initializeRPS() {
    RPS.InitializeTouchMenu();
}

void readPOIs() {
    float A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;

    Debugger::printNextLine("reading data from SD...");

    //READ SD CARD FOR LOGGED X AND Y DATA POINTS
    FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "r");
    SD.FScanf(fptr, "%f%f", &A_x, &A_y);
    SD.FScanf(fptr, "%f%f", &B_x, &B_y);
    SD.FScanf(fptr, "%f%f", &C_x, &C_y);
    SD.FScanf(fptr, "%f%f", &D_x, &D_y);
    SD.FClose(fptr);

    Debugger::printNextLine("success.");
    Debugger::printNextLine("A: (%f, %f)", A_x, A_y);
    Debugger::printNextLine("B: (%f, %f)", B_x, B_y);
    Debugger::printNextLine("C: (%f, %f)", C_x, C_y);
    Debugger::printNextLine("D: (%f, %f)", D_x, D_y);
}

void readResults() { // Quite possibly does not work
    char str[287] = {0};
    FEHFile* resultsFile = SD.FOpen("RESULTS.txt", "r");
    SD.FScanf(resultsFile, "%286c", str); // this is kinda sketchy tbh
    SD.FClose(resultsFile);
    Debugger::printWrap(1, "%s", str);
    if (strlen(str) < 286) Debugger::printNextLine("[end of file]");
}

void displayRPSInfo() {
    while (true) {
        Debugger::printLine(1, "   Region: %c", RPS.CurrentRegionLetter());
        Debugger::printLine(2, "Time left: %i", RPS.Time());
        Debugger::printLine(3, "    Lever: %i", RPS.GetCorrectLever());
        Debugger::printLine(5, "        X: %f", RPS.X());
        Debugger::printLine(6, "        Y: %f", RPS.Y());
        Debugger::printLine(7, "  Heading: %f", RPS.Heading());
        Debugger::sleep(0.05);
    }
}

int main()
{
    ProteOS::registerFunction("initializeRPS()", &initializeRPS);
    ProteOS::registerFunction("setPOIs()", &setPOIs);
    ProteOS::registerFunction("goToPOIs()", &goToPOIs);
    ProteOS::registerFunction("readPOIs()", &readPOIs);
    ProteOS::registerFunction("readResults()", &readResults);
    ProteOS::registerFunction("displayRPSInfo()", &displayRPSInfo);

    ProteOS::run();
}
