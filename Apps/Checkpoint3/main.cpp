#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHLCD.h"
#include "FEHServo.h"


FEHServo mouthServo(FEHServo::Servo0);


void calibrateServo();
void moveServo();

int main() {
    mouthServo.SetMin(500);
    mouthServo.SetMax(2390);
    ProteOS::registerFunction("calibrateServo()", &calibrateServo);
    ProteOS::registerFunction("moveServo()", &moveServo);

    ProteOS::run();
}


void calibrateServo() {
    mouthServo.TouchCalibrate();
}

void moveServo() {
    float servoAngle = 0;
    Debugger::printWrap(4, "Touch the left side of the screen to move the servo left, and the right side to move it right");
    while (true) {
        Debugger::printLine(2, "Servo angle: %f", servoAngle);
        mouthServo.SetDegree(servoAngle);
        float x, y;
        if (LCD.Touch(&x, &y)) {
            if (x < 160) {
                servoAngle -= 0.25f * y;
                if (servoAngle < 0) servoAngle = 0;
            } else {
                servoAngle += 0.25f * y;
                if (servoAngle > 180) servoAngle = 180;
            }
        }
        Debugger::sleep(0.05f);
    }
}
