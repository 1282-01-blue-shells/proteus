#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include "FEHMotor.h"
#include "FEHIO.h"

void setMotors(FEHMotor* leftMotor, FEHMotor* rightMotor);
void setEncoders(DigitalEncoder* leftEncoder, DigitalEncoder* rightEncoder);

void turn(float degrees, float maxPower);

#endif