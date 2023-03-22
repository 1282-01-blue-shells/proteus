#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include "FEHMotor.h"
#include "FEHIO.h"

// Constants for motor and encoder setup. Can be changed if needed
#define MOTOR_VOLTAGE 9.0f
#define LEFT_MOTOR_PORT FEHMotor::Motor0
#define RIGHT_MOTOR_PORT FEHMotor::Motor1
#define LEFT_ENCODER_PIN FEHIO::P0_0
#define RIGHT_ENCODER_PIN FEHIO::P0_1

// Constants for encoder count calculation
#define ENCODER_COUNTS_PER_DEGREE 2.65f
#define ENCODER_COUNTS_PER_INCH 40.489f

// Constants for calculation of slowdown
#define SLOWDOWN_THRESHOLD_COEFFICIENT 2.0f
#define SLOWDOWN_POWER_REDUCTION 0.4f
#define SLOWDOWN_DISTANCE_REDUCTION 0.4f

#define QRCODE_DEFAULT_X 5.6f
#define QRCODE_DEFAULT_Y 0.5f
#define QRCODE_DEFAULT_A -3.18f

class Motors {
public:

    // Members //

    // Maximum power percentage to supply to the motors.
    // Default: 40
    // Recommended: 25 - 60
    static float maxPower;

    // The ratio of power given to the right motor vs power given to the left motor. If
    //   the right motor needs more power to move at the same speed as the left, then 
    //   increase this number, and vice versa.
    // Default: 1.0
    // Recommended: 0.8 - 1.25
    static float motorPowerRatio;

    // How many times the motors should slow down during a single movement to avoid 
    //   overshooting. Higher values increase accuracy and decrease speed.
    // Default: 1
    // Reccommended: 0 - 2
    static int slowdownStages;

    // How long in seconds the robot should wait before starting to turn or drive. If
    //   this is too low, inertia from previous motor movements might be read by the
    //   encoders, reducing accuracy. 
    // Default: 0.2
    // Recommended: 0.0 - 0.4
    static float delay;

    // How long in seconds the robot should wait to stabilize before calling RPS, in 
    //   order to avoid latency issues.
    // Default: 0.4
    // Recommended: 0.2 - 0.6
    static float rpsDelay;

    // Motors and encoders. These will be given a value before the program starts, so
    //   constructing your own motor or encoder objects is not necessary.
    static FEHMotor lMotor, rMotor;
    static DigitalEncoder lEncoder, rEncoder;

    // Relative position and rotation of the QR code.
    static float qrCodeX, qrCodeY, qrCodeA;


    // Functions //

    // Turns the specified angle in terms of heading. A positive angle will turn to the
    //   right, and a negative angle will turn to the left.
    static void turn(float degrees);

    // Drives the specified distance in inches. If distance is negative, it will drive
    //   backwards instead.
    static void drive(float distance);

    // Starts both motors going forward.
    static void start();

    // Starts both motors. If forward is true, it will drive forwards, and otherwise it
    //   will drive backwards.
    static void start(bool forward);

    // Stops both motors.
    static void stop();

    // Moves the robot and communicates with RPS in order to calculate the position and
    //   rotation of the QR code, relative to the robot's center of rotation. Result is
    //   displayed to the screen and stored in the appropriate variables. If an error
    //   occurs, returns -1 instead.
    static int calibrateQRCode();

    // Uses RPS to determine the robot's position. On success, this will set the values
    //   and return a success code of 0. On error, no values will be set and an error
    //   code will be returned:
    // -1: RPS is connected, but the QR code cannot be found on the course.
    // -2: The QR code was found, but is in the deadzone.
    // -3: RPS is not connected.
    static int getCurrentPos(float* x, float* y, float* h);

    // Uses RPS to drive to the specified absolute position and rotation. Does not 
    //   check actual position or consider obstacles. Returns appropriate error codes on
    //   failure, and 0 on success.
    static int driveTo(float targetX, float targetY, float targetH);


private:
    static void calculateMotorPower(float* leftPower, float* rightPower);
    static void doMovementWithSlowdown(float leftPower, float rightPower, int distanceInCounts);
};

#endif
