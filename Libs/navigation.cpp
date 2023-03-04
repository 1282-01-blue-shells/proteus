#include "navigation.hpp"

#include "proteos.hpp"
#include "math.h"

float Motors::maxPower = 40;
float Motors::motorPowerRatio = 1.0f;
int Motors::slowdownStages = 1;
float Motors::delay = 0.2f;

FEHMotor Motors::lMotor(LEFT_MOTOR_PORT, MOTOR_VOLTAGE);
FEHMotor Motors::rMotor(RIGHT_MOTOR_PORT, MOTOR_VOLTAGE);
DigitalEncoder Motors::lEncoder(LEFT_ENCODER_PIN);
DigitalEncoder Motors::rEncoder(RIGHT_ENCODER_PIN);



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

    // The motors will slow down when there are this many counts until the end
    int slowdownDistance = distanceInCounts - (int)(maxPower * SLOWDOWN_THRESHOLD_COEFFICIENT);

    lEncoder.ResetCounts();
    rEncoder.ResetCounts();
    lMotor.SetPercent(leftPower);
    rMotor.SetPercent(rightPower);

    // Loop for however many slowdown stages there are. If there are none, this loop will be skipped
    for (int i = 0; i < slowdownStages; i++) {

        // Wait until the appropriate distance
        while ((lEncoder.Counts() + rEncoder.Counts()) / 2 < distanceInCounts - slowdownDistance) {
            abortCheck();
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
        abortCheck();
    }

    // We have arrived, stop motors
    Motors::stop();
}

void Motors::turn(float degrees) {
    sleepWithAbortCheck(delay);

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
    sleepWithAbortCheck(delay);

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



/* void turn(float degrees, float maxPower) {
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

    leftMotor_->SetPercent(leftPower * SLOW_DOWN);
    rightMotor_->SetPercent(rightPower * SLOW_DOWN);

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

    leftMotor_->SetPercent(leftPower * SLOW_DOWN);
    rightMotor_->SetPercent(rightPower * SLOW_DOWN);

    while ((leftEncoder_->Counts() + rightEncoder_->Counts()) / 2 < counts) {
        abortCheck();
    }

    leftMotor_->Stop();
    rightMotor_->Stop();
} */

