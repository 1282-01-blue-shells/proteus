#include "drivetrain.hpp"

namespace dt {

const PortConfig PortConfig::STANDARD = {
    FEHMotorPort::Motor0,
    FEHMotorPort::Motor1,
    FEHIOPin::P1_0,
    FEHIOPin::P1_1,
};

constexpr auto MOTOR_VOLTAGE = 9.f;

Drivetrain::Drivetrain() noexcept : Drivetrain{PortConfig::STANDARD} {}

Drivetrain::Drivetrain(PortConfig config) noexcept
: left{
    FEHMotor::FEHMotor(config.leftMotor, MOTOR_VOLTAGE),
    DigitalEncoder::DigitalEncoder(config.leftEncoder)
},
right {
    FEHMotor::FEHMotor(config.rightMotor, MOTOR_VOLTAGE),
    DigitalEncoder::DigitalEncoder(config.rightEncoder)
} {}

void Drivetrain::turn(const float degrees) {
    // TODO
}

void Drivetrain::stop() {
    this->left.stop();
    this->right.stop();
}

Motor::Motor(const FEHMotor &&inner, const DigitalEncoder &&encoder)
: inner{inner}, encoder{encoder}
{
    this->encoder.ResetCounts();
}

void Motor::set(const float power) {
    this->encoder.ResetCounts();
    this->inner.SetPercent(power);
}

void Motor::stop() {
    this->inner.Stop();
}

constexpr auto COUNTS_PER_INCH = 40.489f;

float Motor::inchesTraveled() {
    return static_cast<float>(this->encoder.Counts()) / COUNTS_PER_INCH;
}

void Motor::waitUntilInchesTraveled(const float inches) {
    const auto minCounts = inches * COUNTS_PER_INCH;
    while (this->encoder.Counts() < minCounts) {}
}

} // namespace dt
