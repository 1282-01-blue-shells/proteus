#include "navigation.hpp"

#include "proteos.hpp"
#include "math.h"

#define TRACK_RADIUS 4.0f

#define MOTOR_POWER_RATIO 1.0f
#define ENCODER_COUNTS_PER_DEGREE 2.8f
#define ENCODER_COUNTS_PER_INCH 40.489f

#define DELAY 0.2f

#define SLOW_DOWN_THRESHOLD_COEFFICIENT 2.0f
#define SLOW_DOWN_COEFFICIENT 0.5f


FEHMotor* leftMotor_;
FEHMotor* rightMotor_;
DigitalEncoder* leftEncoder_;
DigitalEncoder* rightEncoder_;

void displayError() {
    setDebuggerFontColor(0xFF0000);
    printLineF(12, "Motors/Encoders not set");
}

void setMotors(FEHMotor* leftMotor__, FEHMotor* rightMotor__) {
    leftMotor_ = leftMotor__;
    rightMotor_ = rightMotor__;
}

void setEncoders(DigitalEncoder* leftEncoder__, DigitalEncoder* rightEncoder__) {
    leftEncoder_ = leftEncoder__;
    rightEncoder_ = rightEncoder__;
}

bool hasNullRef() {
    return (leftMotor_ == NULL || rightMotor_ == NULL || leftEncoder_ == NULL || rightEncoder_ == NULL);
}

void turn(float degrees, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }
    sleepWithAbortCheck(DELAY);

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

    leftEncoder_->ResetCounts();
    rightEncoder_->ResetCounts();
    leftMotor_->SetPercent(leftPower);
    rightMotor_->SetPercent(rightPower);

    while ((leftEncoder_->Counts() + rightEncoder_->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor_->Stop();
    rightMotor_->Stop();
}

void turnWithSlowdown(float degrees, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }
    sleepWithAbortCheck(DELAY);

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

    leftEncoder_->ResetCounts();
    rightEncoder_->ResetCounts();
    leftMotor_->SetPercent(leftPower);
    rightMotor_->SetPercent(rightPower);

    while ((leftEncoder_->Counts() + rightEncoder_->Counts()) / 2 < slowDownThreshold) {
        abortCheck();
    }

    leftMotor_->SetPercent(leftPower * SLOW_DOWN_COEFFICIENT);
    rightMotor_->SetPercent(rightPower * SLOW_DOWN_COEFFICIENT);

    while ((leftEncoder_->Counts() + rightEncoder_->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor_->Stop();
    rightMotor_->Stop();
}

void drive(float distance, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }
    sleepWithAbortCheck(DELAY);

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    int counts = (int) (distance * ENCODER_COUNTS_PER_INCH);

    leftEncoder_->ResetCounts();
    rightEncoder_->ResetCounts();
    leftMotor_->SetPercent(leftPower);
    rightMotor_->SetPercent(rightPower);

    while ((leftEncoder_->Counts() + rightEncoder_->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor_->Stop();
    rightMotor_->Stop();
}

void startDriving(float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }
    sleepWithAbortCheck(DELAY);

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    leftMotor_->SetPercent(leftPower);
    rightMotor_->SetPercent(rightPower);
}

void stopDriving() {
    if (hasNullRef()) {
        displayError();
        return;
    }

    leftMotor_->Stop();
    rightMotor_->Stop();
}

void driveWithSlowdown(float distance, float maxPower) {
    if (hasNullRef()) {
        displayError();
        return;
    }
    sleepWithAbortCheck(DELAY);

    float leftPower = maxPower, rightPower = maxPower;

    if (MOTOR_POWER_RATIO < 1) {
        rightPower *= MOTOR_POWER_RATIO;
    } else {
        leftPower /= MOTOR_POWER_RATIO;
    }

    int counts = (int) (distance * ENCODER_COUNTS_PER_INCH);
    int slowDownThreshold = counts - (int)(maxPower * SLOW_DOWN_THRESHOLD_COEFFICIENT);

    leftEncoder_->ResetCounts();
    rightEncoder_->ResetCounts();
    leftMotor_->SetPercent(leftPower);
    rightMotor_->SetPercent(rightPower);

    while ((leftEncoder_->Counts() + rightEncoder_->Counts()) / 2 < slowDownThreshold) {
        abortCheck();
    }

    leftMotor_->SetPercent(leftPower * SLOW_DOWN_COEFFICIENT);
    rightMotor_->SetPercent(rightPower * SLOW_DOWN_COEFFICIENT);

    while ((leftEncoder_->Counts() + rightEncoder_->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor_->Stop();
    rightMotor_->Stop();
}

