#include "navigation.hpp"

#include "FEHRPS.h"
#include "FEHUtility.h"

#include "math.h"

#include "debugger.hpp"

// why do I have to define this myself this is dumb
#define M_PI 3.1415926535f

#define DEG_TO_RAD (M_PI / 180)
#define RAD_TO_DEG (180 / M_PI)

// Lord help me    // spoiler alert: he did not help me
/* class Vector2 {
public:
    float x, y;

    Vector2() { x = 0; y = 0; }
    Vector2(float a) { x = cos(a); y = sin(a); }
    Vector2(float x_, float y_) { x = x_; y = y_; }

    void scale(float s) {
        x *= s;
        y *= s;
    }
    float radius() {
        return sqrt(x*x + y*y);
    }
    void normalize() {
        this->scale(1/radius());
    }

    void rotateByAngle(float a) {
        float newX = x*cos(a) - y*sin(a);
        y = x*sin(a) + y*cos(a);
        x = newX;
    }
    void rotateByUnitVector(Vector2* o) {
        float newX = x*o->x - y*o->y; // good lord there's matrickse
        y = x*o->y + y*o->x;
        x = newX;
    }
    float dotProduct(Vector2* o) {
        return (x * o->x) + (y * o->y);
    }
    float crossProduct(Vector2* o) {
        return (x * o->y) - (y * o->x);
    }
    float angleDifference(Vector2* o) {
        float angle = asin(crossProduct(o));
        float dotP = dotProduct(o);
        if (dotP < 0) {
            if (angle > 0) {
                angle = M_PI - angle;
            } else {
                angle = -M_PI + angle;
            }
        }
        return angle;
    }
}; */


// Static variable definitions

float Motors::maxPower = 40;
float Motors::motorPowerRatio = 1.0f;
int Motors::slowdownStages = 1;
float Motors::delay = 0.2f;
float Motors::rpsDelay = 0.3f;
float Motors::movementTimeoutPerInch = 0.2f;

/* float Motors::qrCodeX = QRCODE_DEFAULT_X;
float Motors::qrCodeY = QRCODE_DEFAULT_Y;
float Motors::qrCodeA = QRCODE_DEFAULT_A; */

FEHMotor Motors::lMotor(LEFT_MOTOR_PORT, MOTOR_VOLTAGE);
FEHMotor Motors::rMotor(RIGHT_MOTOR_PORT, MOTOR_VOLTAGE);
DigitalEncoder Motors::lEncoder(LEFT_ENCODER_PIN);
DigitalEncoder Motors::rEncoder(RIGHT_ENCODER_PIN);


// Function definitions

void Motors::calculateMotorPower(float* leftPower, float* rightPower) {
    *leftPower = maxPower;
    *rightPower = maxPower;
    if (motorPowerRatio < 1) {
        *rightPower *= motorPowerRatio;
    } else {
        *leftPower /= motorPowerRatio;
    }
}

void Motors::doMovementWithSlowdown(float leftPower, float rightPower, int distanceInCounts) {
    // floating point inaccuracies dictate that this will wait a few seconds less than 
    //   expected if the proteus is left on for 194 days
    // but if you look at the libraries the integer part of the time in seconds is 
    //   actually a uint16. So if you leave your proteus on for 18 hours, 12 minutes,
    //   and 15 seconds, and then start a motor movement, it will overflow and the
    //   timeout will never happen
    float timeoutTime = (float) TimeNow() + movementTimeoutPerInch * distanceInCounts / ENCODER_COUNTS_PER_INCH;

    // The motors will slow down when there are this many counts until the end
    float slowdownDistance = maxPower * SLOWDOWN_THRESHOLD_COEFFICIENT;

    lEncoder.ResetCounts();
    rEncoder.ResetCounts();
    lMotor.SetPercent(leftPower);
    rMotor.SetPercent(rightPower);

    // Loop for however many slowdown stages there are. If there are none, this loop will be skipped
    for (int i = 0; i < slowdownStages; i++) {

        // Wait until the appropriate distance
        while ((lEncoder.Counts() + rEncoder.Counts()) / 2 < distanceInCounts - (int)slowdownDistance) {
            Debugger::abortCheck();
            if (TimeNow() > timeoutTime) break;
        }

        // Robot should now slow down

        // Decrease motor power
        leftPower *= SLOWDOWN_POWER_REDUCTION;
        rightPower *= SLOWDOWN_POWER_REDUCTION;
        lMotor.SetPercent(leftPower);
        rMotor.SetPercent(rightPower);

        // Decrease the distance for the next stage, if there is one
        slowdownDistance *= SLOWDOWN_DISTANCE_REDUCTION;
    }

    // Done with slowdown stages, wait for final stage to reach the end
    while ((lEncoder.Counts() + rEncoder.Counts()) / 2 < distanceInCounts) {
        Debugger::abortCheck();
        if (TimeNow() > timeoutTime) break;
    }

    // We have arrived, stop motors
    Motors::stop();
}

void Motors::turn(float degrees) {
    Debugger::sleep(delay);

    float leftPower, rightPower;
    calculateMotorPower(&leftPower, &rightPower);

    // One motor will be going backwards
    if (degrees < 0) {
        leftPower *= -1;
    } else {
        rightPower *= -1;
    }

    int totalDistanceInCounts = (int) (abs(degrees) * ENCODER_COUNTS_PER_DEGREE);

    doMovementWithSlowdown(leftPower, rightPower, totalDistanceInCounts);
}

void Motors::drive(float distance) {
    Debugger::sleep(delay);

    float leftPower, rightPower;
    calculateMotorPower(&leftPower, &rightPower);

    // If the distance is negative, we should drive backwards instead
    if (distance < 0) {
        leftPower *= -1;
        rightPower *= -1;
    }

    int totalDistanceInCounts = (int) (abs(distance) * ENCODER_COUNTS_PER_INCH);

    doMovementWithSlowdown(leftPower, rightPower, totalDistanceInCounts);
}

void Motors::pulse_forward(int percent, float seconds){
    
    lMotor.SetPercent(percent);
    rMotor.SetPercent(percent);

    Sleep(seconds);

    //stop motors after pulse is complete
    lMotor.Stop();
    rMotor.Stop();
}

void Motors::pulse_counterclockwise(int percent, float seconds)
{
    lMotor.SetPercent(-percent);
    rMotor.SetPercent(percent);

    Sleep(seconds);

    //stop motors after pulse is complete
    lMotor.Stop();
    rMotor.Stop();
}

void Motors::start() {
    Motors::start(true);
}

void Motors::start(bool forward) {
    float leftPower, rightPower;
    calculateMotorPower(&leftPower, &rightPower);

    // If forward is false, we should drive backwards instead
    if (!forward) {
        leftPower *= -1;
        rightPower *= -1;
    }

    lMotor.SetPercent(leftPower);
    rMotor.SetPercent(rightPower);
}

void Motors::stop() {
    lMotor.Stop();
    rMotor.Stop();
}

/* int Motors::calibrateQRCode() {
    float x1, x2, x3,  y1, y2, y3,  a1;
    qrCodeX = 0;
    qrCodeY = 0;
    qrCodeA = 0;

    // get the robot's current position (and return early if there's an error)
    if (getCurrentPos(&x1, &y1, &a1)) return -1;
    // convert to radians
    a1 *= DEG_TO_RAD;

    // initialize a unit vector pointing in the same direction as the qr code
    Vector2 qrCodeFacingDirection(a1);

    // drive forward 8 inches and get the new position
    drive(8);
    if (getCurrentPos(&x2, &y2, NULL)) return -1;

    // initialize a vector pointing in the direction the robot moved, then normalize it
    Vector2 robotFacingDirection(x2-x1, y2-y1);
    robotFacingDirection.normalize();

    // get the cross product
    //float crossProduct = robotFacingDirection.crossProduct(&qrCodeFacingDirection);

    // the angle of displacement between the qr code's and the robot's facing directions
    float angleDisp = robotFacingDirection.angleDifference(&qrCodeFacingDirection);
    // this will be the relative angle of the qr code


    // turn around and get new position
    turn(180);
    if (getCurrentPos(&x3, &y3, NULL)) return -1;

    // displacement between the old position and the new position
    Vector2 qrCodeDisplacement(x2-x3, y2-y3);

    // scale by 0.5 to get radius instead of diameter
    qrCodeDisplacement.scale(0.5);

    // (reflect to negate angle)
    robotFacingDirection.y *= -1;
    // rotate displacement to be relative to the robot's direction
    qrCodeDisplacement.rotateByUnitVector(&robotFacingDirection);
    // this will be the relative position of the qr code


    qrCodeX = qrCodeDisplacement.x;
    qrCodeY = qrCodeDisplacement.y;
    qrCodeA = angleDisp;

    Debugger::printNextLine("QR Code X: %f", qrCodeX);
    Debugger::printNextLine("QR Code Y: %f", qrCodeY);
    Debugger::printNextLine("QR Code A: %f", qrCodeA);
    return 0;
}

int Motors::getCurrentPos(float* x, float* y, float* h) {
    Debugger::sleep(rpsDelay);

    // RPS region number is set to -1 by default, and then set to the proper value after
    //   connecting. If it is still -1, RPS is not connected.
    if (RPS.CurrentRegion() < 0) {
        return -3;
    }

    float readX = RPS.X();
    float readY = RPS.Y();
    float readH = RPS.Heading();

    // someone decided these functions should return APPROXIMATELY the correct error code
    //   so now I have to check entire ranges for if there was an error
    if (readX <= -1.5 || readY <= -1.5 || readH <= -1.5) {
        return -2;
    }
    if (readX < 0 || readY < 0 || readH < 0) {
        return -1;
    }
    // Otherwise, there was no error and we can continue

    // calculate angle
    float angle = readH * DEG_TO_RAD - qrCodeA;

    // rotate qr code relative position to calculate absolute position offset
    float xOffset = qrCodeX * cos(angle) - qrCodeY * sin(angle);
    float yOffset = qrCodeX * sin(angle) + qrCodeY * cos(angle);

    // calculate actual position and heading
    float actualX = readX - xOffset;
    float actualY = readY - yOffset;
    float actualH = angle * RAD_TO_DEG;

    // (wrap the heading to be between 0 and 360)
    if (actualH < 0) actualH += 360;
    if (actualH >= 360) actualH -= 360;

    // set the x, y, and h to the correct values and then return
    if (x != NULL) *x = actualX;
    if (y != NULL) *y = actualY;
    if (h != NULL) *h = actualH;
    return 0;
}

int Motors::driveTo(float targetX, float targetY, float targetH) {
    float currentX, currentY, currentH;

    // get the current position and rotation (and return early if error)
    int err = getCurrentPos(&currentX, &currentY, &currentH);
    if (err) return err;

    // get displacement between the target position and the current position
    Vector2 displacement(targetX - currentX, targetY - currentY);

    // calculate distance between the points (used later)
    float distance = displacement.radius();

    // normalize for cross product
    displacement.normalize();
    // get the direction the robot is currently facing
    Vector2 currentFacingDirection(currentH * DEG_TO_RAD);
    
    // determine difference in angle to point in the correct direction
    float angleDisp = currentFacingDirection.angleDifference(&displacement);

    // convert the angle to rightwards degrees and then turn the correct amount
    turn(angleDisp * -RAD_TO_DEG);

    // drive to the target point
    drive(distance);

    // some more inverse trigonometry cross product shenanigans
    Vector2 targetFacingDirection(targetH * DEG_TO_RAD);
    angleDisp = displacement.angleDifference(&targetFacingDirection);
    // turn to the intended final angle
    turn(angleDisp * -RAD_TO_DEG);

    return 0;
}

int Motors::driveToBackwards(float targetX, float targetY, float targetH) {
    float currentX, currentY, currentH;

    // get the current position and rotation (and return early if error)
    int err = getCurrentPos(&currentX, &currentY, &currentH);
    if (err) return err;

    // get displacement between the target position and the current position (but backwards)
    Vector2 displacement(currentX - targetX, currentY - targetY);

    // calculate distance between the points (used later)
    float distance = displacement.radius();

    // normalize for cross product
    displacement.normalize();
    // get the direction the robot is currently facing
    Vector2 currentFacingDirection(currentH * DEG_TO_RAD);
    
    // determine difference in angle to point in the correct direction
    float angleDisp = currentFacingDirection.angleDifference(&displacement);

    // convert the angle to rightwards degrees and then turn the correct amount
    turn(angleDisp * -RAD_TO_DEG);

    // drive to the target point
    drive(-distance);

    // some more inverse trigonometry cross product shenanigans
    Vector2 targetFacingDirection(targetH * DEG_TO_RAD);
    angleDisp = displacement.angleDifference(&targetFacingDirection);
    // turn to the intended final angle
    turn(angleDisp * -RAD_TO_DEG);

    return 0;
} */

// helper function, wraps angle around to be between -180 and 180
float limitAngle(float a) {
    while (a < -180) a += 360;
    while (a >= 180) a -= 360;
}

// "error-type" "internal error" hey compiler how about you shut th
float absVal(float a) {
    return (a < 0) ? -a : a;
}

// turns the robot to the specified heading using RPS
void Motors::lineUpToAngle(float targetH) {

    Debugger::printLine(1, "turning to h = %.1f", targetH);

    float currentH = RPS.Heading();
    while (absVal(limitAngle(targetH - currentH)) > ERROR_THRESHOLD_DEGREES) {

        Debugger::printLine(2, "targ: %.1f curr: %.1f", targetH, currentH);
        Debugger::printLine(3, "error: %.1f", limitAngle(targetH - currentH));

        Motors::turn(-limitAngle(targetH - currentH));
        Debugger::sleep(rpsDelay);
        currentH = RPS.Heading();
    }

    Debugger::printLine(2, "targ: %.1f curr: %.1f", targetH, currentH);
    Debugger::printLine(3, "error: %.1f", limitAngle(targetH - currentH));
    Debugger::printLine(4, "Finished");
}

// moves the robot along its current facing axis until it reaches the specified x
//   coordinate. works best if lined up with the x axis
void Motors::lineUpToXCoordinate(float x) {

    Debugger::printLine(1, "going to x = %.1f", x);

    // account for how far forward the QR code is on the robot
    float targetX = x + QRCODE_OFFSET * cos(RPS.Heading() * DEG_TO_RAD);

    // repeat until close to the target position
    float currentX = RPS.X();
    while (abs(targetX - currentX) > ERROR_THRESHOLD_INCHES) {

        Debugger::printLine(2, "targ: %.1f curr: %.1f", targetX, currentX);
        Debugger::printLine(3, "error: %.1f", abs(targetX - currentX));

        // drive towards the target position (accounting for the robot's facing direction)
        Motors::drive((targetX - currentX) / cos(RPS.Heading() * DEG_TO_RAD));

        // update position variable
        Debugger::sleep(rpsDelay);
        currentX = RPS.X();
        targetX = x + QRCODE_OFFSET * cos(RPS.Heading() * DEG_TO_RAD);
    }

    Debugger::printLine(2, "targ: %.1f curr: %.1f", targetX, currentX);
    Debugger::printLine(3, "error: %.1f", abs(targetX - currentX));
    Debugger::printLine(4, "Finished");
}

// moves the robot along its current facing axis until it reaches the specified y
//   coordinate. works best if lined up with the y axis
void Motors::lineUpToYCoordinate(float y) {

    Debugger::printLine(1, "going to y = %.1f", y);

    // account for how far forward the QR code is on the robot
    float targetY = y + QRCODE_OFFSET * sin(RPS.Heading() * DEG_TO_RAD);

    // repeat until close to the target position
    float currentY = RPS.X();
    while (abs(targetY - currentY) > ERROR_THRESHOLD_INCHES) {

        Debugger::printLine(2, "targ: %.1f curr: %.1f", targetY, currentY);
        Debugger::printLine(3, "error: %.1f", abs(targetY - currentY));

        // drive towards the target position (accounting for the robot's facing direction)
        Motors::drive((targetY - currentY) / sin(RPS.Heading() * DEG_TO_RAD));

        // update position variable
        Debugger::sleep(rpsDelay);
        currentY = RPS.Y();
        targetY = y + QRCODE_OFFSET * sin(RPS.Heading() * DEG_TO_RAD);
    }

    Debugger::printLine(2, "targ: %.1f curr: %.1f", targetY, currentY);
    Debugger::printLine(3, "error: %.1f", abs(targetY - currentY));
    Debugger::printLine(4, "Finished");
}
