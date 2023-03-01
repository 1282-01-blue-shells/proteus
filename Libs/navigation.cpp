#include "navigation.hpp"

#include "proteos.hpp"
#include "math.h"

#define TRACK_RADIUS 4.0f

#define MOTOR_POWER_RATIO 1.0f
#define ENCODER_COUNTS_PER_DEGREE 2.8f
#define ENCODER_COUNTS_PER_INCH 40.489f

#define SLOW_DOWN_THRESHOLD_COEFFICIENT 2.0f
#define SLOW_DOWN_COEFFICIENT 0.5f

#define WEIGHT_P 1.0f
#define WEIGHT_I 0.1f
#define WEIGHT_D -1.0f


FEHMotor* leftMotor;
FEHMotor* rightMotor;
DigitalEncoder* leftEncoder;
DigitalEncoder* rightEncoder;

void displayError() {
    setDebuggerFontColor(0xFF0000);
    printLineF(12, "Motors/Encoders not set");
}

void setMotors(FEHMotor* leftMotor_, FEHMotor* rightMotor_) {
    leftMotor = leftMotor_;
    rightMotor = rightMotor_;
}

void setEncoders(DigitalEncoder* leftEncoder_, DigitalEncoder* rightEncoder_) {
    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;
}

bool hasNullRef() {
    return (leftMotor == NULL || rightMotor == NULL || leftEncoder == NULL || rightEncoder == NULL);
}

void turn(float degrees, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }
    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    if (degrees < 0) {
        leftPower *= -1;
        degrees *= -1;
    } else {
        rightPower *= -1;
    }

    int counts = (int) (degrees * ENCODER_COUNTS_PER_DEGREE);

    leftEncoder->ResetCounts();
    rightEncoder->ResetCounts();
    leftMotor->SetPercent(leftPower);
    rightMotor->SetPercent(rightPower);

    while ((leftEncoder->Counts() + rightEncoder->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor->Stop();
    rightMotor->Stop();
}

void turnWithSlowdown(float degrees, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    if (degrees < 0) {
        leftPower *= -1;
        degrees *= -1;
    } else {
        rightPower *= -1;
    }

    int counts = (int) (degrees * ENCODER_COUNTS_PER_DEGREE);
    int slowDownThreshold = counts - (int)(maxPower * SLOW_DOWN_THRESHOLD_COEFFICIENT);

    leftEncoder->ResetCounts();
    rightEncoder->ResetCounts();
    leftMotor->SetPercent(leftPower);
    rightMotor->SetPercent(rightPower);

    while ((leftEncoder->Counts() + rightEncoder->Counts()) / 2 < slowDownThreshold) {
        abortCheck();
    }

    leftMotor->SetPercent(leftPower * SLOW_DOWN_COEFFICIENT);
    rightMotor->SetPercent(rightPower * SLOW_DOWN_COEFFICIENT);

    while ((leftEncoder->Counts() + rightEncoder->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor->Stop();
    rightMotor->Stop();
}

void drive(float distance, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    int counts = (int) (distance * ENCODER_COUNTS_PER_INCH);

    leftEncoder->ResetCounts();
    rightEncoder->ResetCounts();
    leftMotor->SetPercent(leftPower);
    rightMotor->SetPercent(rightPower);

    while ((leftEncoder->Counts() + rightEncoder->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor->Stop();
    rightMotor->Stop();
}

void startDriving(float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    leftMotor->SetPercent(leftPower);
    rightMotor->SetPercent(rightPower);
}

void stopDriving() {
    if (hasNullRef()) {
        displayError();
        return;
    }

    leftMotor->Stop();
    rightMotor->Stop();
}

void driveWithSlowdown(float distance, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    int counts = (int) (distance * ENCODER_COUNTS_PER_INCH);
    int slowDownThreshold = counts - (int)(maxPower * SLOW_DOWN_THRESHOLD_COEFFICIENT);

    leftEncoder->ResetCounts();
    rightEncoder->ResetCounts();
    leftMotor->SetPercent(leftPower);
    rightMotor->SetPercent(rightPower);

    while ((leftEncoder->Counts() + rightEncoder->Counts()) / 2 < slowDownThreshold) {
        abortCheck();
    }

    leftMotor->SetPercent(leftPower * SLOW_DOWN_COEFFICIENT);
    rightMotor->SetPercent(rightPower * SLOW_DOWN_COEFFICIENT);

    while ((leftEncoder->Counts() + rightEncoder->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor->Stop();
    rightMotor->Stop();
}


// WIP

int encoderDiff(DigitalEncoder* encoder, int* counts) {
    int newCounts = encoder->Counts();
    int difference = newCounts - *counts;
    *counts = newCounts;
    return difference;
}

void updatePos(int leftCounts, int rightCounts, float* x, float* y, float* a) {
    float aDiff = (rightCounts - leftCounts) / ENCODER_COUNTS_PER_DEGREE * (M_PI / 180);

    float relXDiff, relYDiff;

    if (leftCounts == rightCounts) {
        // trivial case, but would cause an error in the normal calculation
        relYDiff = 0;
        relXDiff = leftCounts / ENCODER_COUNTS_PER_INCH;
    } else {
        // might divide by zero here
        float ratio = rightCounts / leftCounts;

        float pivotY = TRACK_RADIUS; // appropriate value if it is zero
        if (leftCounts != 0) { // if it isn't, compute it normally
            // the forbidden algebra
            pivotY = (ratio + 1) / (ratio - 1) * TRACK_RADIUS;
        }
        relXDiff = pivotY * sin(aDiff);
        relYDiff = pivotY * (1-cos(aDiff));
    }

    *x += relXDiff * cos(*a) - relYDiff * sin(*a);
    *y += relYDiff * cos(*a) + relXDiff * sin(*a);
    *a += aDiff;
}

/* void driveWithCorrection(float distance, float maxPower) {
    if (hasNullRef()) return;

    float x = 0, y = 0, a = 0; // relative position/rotation
    // robot is considered facing right and wants to go to (distance, 0)
    leftEncoder->ResetCounts();
    rightEncoder->ResetCounts();
    int leftCounts = 0, rightCounts = 0;

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }
} */