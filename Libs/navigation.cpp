#include "navigation.hpp"

#include "math.h"

#include "debugger.hpp"


// Static variable definitions

float Motors::maxPower = 40;
float Motors::motorPowerRatio = 1.0f;
int Motors::slowdownStages = 1;
float Motors::delay = 0.2f;

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

    // The motors will slow down when there are this many counts until the end
    float slowdownDistance = (float)distanceInCounts - (maxPower * SLOWDOWN_THRESHOLD_COEFFICIENT);

    lEncoder.ResetCounts();
    rEncoder.ResetCounts();
    lMotor.SetPercent(leftPower);
    rMotor.SetPercent(rightPower);

    // Loop for however many slowdown stages there are. If there are none, this loop will be skipped
    for (int i = 0; i < slowdownStages; i++) {

        // Wait until the appropriate distance
        while ((lEncoder.Counts() + rEncoder.Counts()) / 2 < distanceInCounts - (int)slowdownDistance) {
            Debugger::abortCheck();
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
