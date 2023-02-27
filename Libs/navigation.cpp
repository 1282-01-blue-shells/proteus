#include "navigation.hpp"
#include "proteos.hpp"

#define MOTOR_POWER_RATIO 1.0f
#define ENCODER_COUNTS_PER_DEGREE 2.8f
#define ENCODER_COUNTS_PER_INCH 40.489f

#define SLOW_DOWN_THRESHOLD_COEFFICIENT 1.0f
#define SLOW_DOWN_COEFFICIENT 0.5f

FEHMotor* leftMotor;
FEHMotor* rightMotor;
DigitalEncoder* leftEncoder;
DigitalEncoder* rightEncoder;

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
    if (hasNullRef()) return;
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
    if (hasNullRef()) return;
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
    if (hasNullRef()) return;
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

void driveWithSlowdown(float distance, float maxPower) {
    if (hasNullRef()) return;
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