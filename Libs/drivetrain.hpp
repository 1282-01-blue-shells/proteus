//! \file
//! \author Will Blankemeyer

#pragma once

#include <FEHIO.h>
#include <FEHMotor.h>

//! \brief The drivetrain.
namespace dt {

using FEHIOPin = FEHIO::FEHIOPin;
using FEHMotorPort = FEHMotor::FEHMotorPort;

//! \brief The port configuration for a drivetrain.
struct PortConfig {
    //! \brief The motor port for the left motor.
    FEHMotorPort leftMotor;
    //! \brief The motor port for the right motor.
    FEHMotorPort rightMotor;
    //! \brief The IO port for the left encoder.
    FEHIOPin leftEncoder;
    //! \brief The IO port for the right encoder.
    FEHIOPin rightEncoder;

    //! \brief The standard drivetrain port configuration.
    static const PortConfig STANDARD;
};

//! \brief A drivetrain.
class Drivetrain {
public:
    //! \brief Creates a new drivetrain with the standard port configuration.
    Drivetrain() noexcept;
    //! \brief Creates a new drivetrain with the given port configuration.
    Drivetrain(PortConfig config) noexcept;

    void turn(float degrees);
    //! \brief Stops all motors.
    void stop();

private:
    Motor left;
    Motor right;
};

//! \brief A DC motor.
class Motor {
public:
    //! \brief Creates a new motor.
    Motor(const FEHMotor &&inner, const DigitalEncoder &&encoder) noexcept;

    void set(float power);
    //! \brief Stops this motor.
    void stop();

    float inchesTraveled();
    void waitUntilInchesTraveled(float inches);

private:
    FEHMotor inner;
    DigitalEncoder encoder;
};

} // namespace dt
