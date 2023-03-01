#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include "FEHMotor.h"
#include "FEHIO.h"

void setMotors(FEHMotor* leftMotor, FEHMotor* rightMotor);
void setEncoders(DigitalEncoder* leftEncoder, DigitalEncoder* rightEncoder);


// turn the specified angle
void turn(float degrees, float maxPower);

// same as turn(), but slows down at the end to avoid overshooting
void turnWithSlowdown(float degrees, float maxPower);

// drive the specified distance in inches
void drive(float distance, float maxPower);

// starts driving without blocking
void startDriving(float maxPower);

// stops driving
void stopDriving();

// same as drive(), but slows down at the end to avoid overshooting
void driveWithSlowdown(float distance, float maxPower);


#endif
