#include "proteos.hpp"
#include "navigation.hpp"

#include "math.h"

#include "FEHLCD.h"
#include "FEHServo.h"
#include "FEHRPS.h"

#define M_PI 3.1415926535f

#define DEG_TO_RAD (M_PI / 180)
#define RAD_TO_DEG (180 / M_PI)



AnalogInputPin lightSensor(FEHIO::P0_7);

FEHServo mouthServo(FEHServo::Servo0);

float leversX = 12, leversY = 23, leversH = 270;
float leverSpacing = -3;



void calibrateServo();
void testServo();
void initializeRPS();
void displayPosition();
void runCheckpoint();
void waitForLight();
void goToLever();
void getCloserToLevers();
void calibrateQRCode();
void testTurn();
void testDrive();

void flipLever(); // flips down then up, tester

void flipLeverDown();
void flipLeverUp();



int main() {
    mouthServo.SetMin(500);
    mouthServo.SetMax(2390);
    
    mouthServo.SetDegree(90); //set to neutral position
    
    ProteOS::registerVariable("leversX", &leversX);
    ProteOS::registerVariable("leversY", &leversY);
    ProteOS::registerVariable("leversH", &leversH);
    ProteOS::registerVariable("leverSpacing", &leverSpacing);
    ProteOS::registerVariable("qrCodeX", &Motors::qrCodeX);
    ProteOS::registerVariable("qrCodeY", &Motors::qrCodeY);
    ProteOS::registerVariable("qrCodeA", &Motors::qrCodeA);
    
    //ProteOS::registerFunction("calibrateServo()", &calibrateServo);
    //ProteOS::registerFunction("testServo()", &testServo);
    //ProteOS::registerFunction("initializeRPS()", &initializeRPS);
    ProteOS::registerFunction("calibrateQRCode()", &calibrateQRCode);
    ProteOS::registerFunction("displayPosition()", &displayPosition);
    ProteOS::registerFunction("runCheckpoint()", &runCheckpoint);
    ProteOS::registerFunction("getCloserToLevers()", &getCloserToLevers);
    ProteOS::registerFunction("goToLever()", &goToLever);
    ProteOS::registerFunction("flipLever()", &flipLever);
    //ProteOS::registerFunction("flipLeverDown()", &flipLeverDown);
    //ProteOS::registerFunction("flipLeverUp()", &flipLeverUp);
    //ProteOS::registerFunction("testTurn()", &testTurn);
    //ProteOS::registerFunction("testDrive()", &testDrive);


    ProteOS::run();
}

// TODO LIST
// 1) Move at the light
// 2) Once get the signal of which fuel pump it is move based off that (done)

void calibrateServo() {
    mouthServo.TouchCalibrate();
}

void calibrateQRCode() {
    Motors::calibrateQRCode();
}

void testServo() {
    float servoAngle = 0;
    Debugger::printWrap(4, "Touch the left side of the screen to move the servo left, and the right side to move it right");
    while (true) {
        Debugger::printLine(2, "Servo angle: %f", servoAngle);
        mouthServo.SetDegree(servoAngle);
        float x, y;
        if (LCD.Touch(&x, &y)) {
            if (x < 160) {
                servoAngle -= 0.1f * y;
                if (servoAngle < 0) servoAngle = 0;
            } else {
                servoAngle += 0.1f * y;
                if (servoAngle > 180) servoAngle = 180;
            }
        }
        Debugger::sleep(0.05f);
    }
}

void initializeRPS() {
    RPS.InitializeTouchMenu();
}

void displayPosition() {
    while (true) {
        float x, y, h;
        Motors::getCurrentPos(&x, &y, &h);
        Debugger::printLine(2, "X: %f", x);
        Debugger::printLine(3, "Y: %f", y);
        Debugger::printLine(4, "H: %f", h);
    }
}

void runCheckpoint() {
    waitForLight();
    getCloserToLevers();
    goToLever();
    flipLever();
}

void waitForLight() {
    Debugger::printLine(9, "Waiting for light...");

    // wait for light
    while (lightSensor.Value()>2); // if no light do nothing


    // Change font color to yellow
    Debugger::setFontColor(0xFFFF00);

    // Print funny message
    Debugger::printWrap(10, "NOW AT LAST I SEEEE THE LIGHT");

    // Reset font color
    Debugger::setFontColor();
}

void getCloserToLevers() {
    Debugger::printNextLine("Getting closer to levers");
    Motors::driveTo(24, 15, 0);
    Motors::driveToBackwards(18, 22, 0);
}

void goToLever() {
    int correctLever = RPS.GetIceCream(); //getting the correct lever position
    float correctLeverPosition;
    if (correctLever == 0) {
        Debugger::printNextLine("Going to left lever");
        correctLeverPosition = leversX;
    } else if (correctLever == 1) {
        Debugger::printNextLine("Going to middle lever");
        correctLeverPosition = leversX + 1*leverSpacing;
    } else if (correctLever == 2) {
        Debugger::printNextLine("Going to right lever");
        correctLeverPosition = leversX + 2*leverSpacing;
    }
    Motors::driveToBackwards(correctLeverPosition, leversY, leversH);
}

void flipLever() {
    flipLeverDown();
    Debugger::sleep(5);
    flipLeverUp();
}

void flipLeverDown() {
    mouthServo.SetDegree(55); //position the lever to be above the lever
    Debugger::sleep(0.5f);
    Debugger::printNextLine("AT 114 Degrees. Now Moving Forward");

    Motors::drive(2); //drive forward to the lever

    mouthServo.SetDegree(90); //bop the lever down
    Debugger::sleep(0.5f);

    // Back up again
    Motors::drive(-2);
}

void flipLeverUp() {
    mouthServo.SetDegree(125); //position lever underneath fuel pump
    Debugger::sleep(0.5f);

    Motors::drive(2); //drive forward again 
    mouthServo.SetDegree(90); //bop lever up 

    Motors::drive(-2);
}


void testTurn() {
    float x, y, h;
    Motors::getCurrentPos(&x, &y, &h);

    float targetX = x;
    float targetY = y;
    float targetH = h + 90;
    if (targetH > 360) targetH -= 360;

    Motors::turn(-90);

    Motors::getCurrentPos(&x, &y, &h);

    Debugger::printNextLine("X: exp %.3f act %.3f", targetX, x);
    Debugger::printNextLine("Y: exp %.3f act %.3f", targetY, y);
    Debugger::printNextLine("H: exp %.3f act %.3f", targetH, h);
}

void testDrive() {
    float x, y, h;
    Motors::getCurrentPos(&x, &y, &h);

    float a = h * DEG_TO_RAD;
    float targetX = x + cos(a)*6;
    float targetY = y + sin(a)*6;
    float targetH = h;

    Motors::drive(6);

    Motors::getCurrentPos(&x, &y, &h);

    Debugger::printNextLine("X: exp %.3f act %.3f", targetX, x);
    Debugger::printNextLine("Y: exp %.3f act %.3f", targetY, y);
    Debugger::printNextLine("H: exp %.3f act %.3f", targetH, h);
}


