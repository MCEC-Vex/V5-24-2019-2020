#include "util.h"

void setupMotors()
{
    // Set motors to hold so they don't move when they're not supposed to
    leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    trayMotorBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    trayMotorFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    leftArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    leftIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    // Right side forward is positive
    rightTopMotor.set_reversed(true);
    rightBottomMotor.set_reversed(true);
    // Right arm going up is positive
    rightArmMotor.set_reversed(true);
    // Right intake intaking is positive
    rightIntake.set_reversed(true);
}

void flipTray()
{
    leftIntake.move(127);
    rightIntake.move(127);

    // Move the arms up and use the pre-made tray movement function to adjust the tray in response
    leftArmMotor.move_absolute(500, 200);
    rightArmMotor.move_absolute(500, 200);
    for(int i = 0; i < 100; i++)
    {
        checkTrayArmsPosOld();
        pros::delay(10);
    }

    // Move the arms back down in the same way
    leftArmMotor.move_absolute(0, 200);
    rightArmMotor.move_absolute(0, 200);
    for(int i = 0; i < 100; i++)
    {
        checkTrayArmsPosOld();
        pros::delay(10);
    }

    leftIntake.move(0);
    rightIntake.move(0);
}

void checkTrayArmsPosOld()
{
    if(leftArmMotor.get_position() < 20)
    {
        return;
    }

    if(leftArmMotor.get_position() < 100)
    {
        setTrayPosition(TRAY_LOWEST + 100, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 200)
    {
        setTrayPosition(TRAY_LOWEST + 250, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 350)
    {
        setTrayPosition(TRAY_LOWEST + 600, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 1500)
    {
        setTrayPosition(TRAY_LOWEST + 1000, TRAY_SHIFT_SPEED);
    }
}

void checkTrayArmsPos()
{
    if(leftArmMotor.get_position() < 20)
    {
        return;
    }
    
    if(leftArmMotor.get_position() < 150)
    {
        setTrayPosition(TRAY_LOWEST + 70, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 350)
    {
        setTrayPosition(TRAY_LOWEST + 200, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 500)
    {
        setTrayPosition(TRAY_LOWEST + 350, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 1500)
    {
        setTrayPosition(TRAY_LOWEST + 1250, TRAY_SHIFT_SPEED);
    }
}

void setTrayPosition(int trayPos, int speed)
{
    trayMotorFront.move_absolute(trayPos, speed);
    trayMotorBack.move_absolute(trayPos, speed);
}

void moveTrayToHighest()
{
    leftIntake.move(-30);
    rightIntake.move(-30);

    while(trayMotorFront.get_position() > TRAY_HIGHEST)
    {
        int speed = 80;
        if(trayMotorFront.get_position() > 1500)
        {
            float segments = TRAY_HIGHEST - 1500;
            speed = 20 + ((60.0 / segments) * (segments - abs(trayMotorFront.get_position() - 1500)));
        }

        trayMotorBack.move_velocity(speed);
        trayMotorFront.move_velocity(speed);
        pros::delay(5);
    }
    trayMotorBack.move_velocity(0);
    trayMotorFront.move_velocity(0);

    leftIntake.move(0);
    rightIntake.move(0);
}

int getUltrasonicDifference()
{
    //return frontUltrasonic.get_value() - backUltrasonic.get_value();
    return 0;
}

double convertToEncoderUnits(std::shared_ptr<okapi::ChassisController> chassis, okapi::QLength distance)
{
    return distance.convert(okapi::meter) *
            chassis->getChassisScales().straight *
            chassis->getGearsetRatioPair().ratio;
}

void waitUntilMotorWithin(pros::Motor motor, int value, int tolerance, int timeout)
{
    double startPos = motor.get_position();
    double distanceFromStart = abs(motor.get_position() - startPos);

    unsigned long current = pros::millis();
    while(abs(abs(motor.get_position() - startPos) - value) > tolerance &&
            (pros::millis() - current) < timeout)
    {
        pros::delay(10);
    }
}

void moveMotors(std::shared_ptr<okapi::ChassisController> chassis, okapi::QLength distance, int speed, int timeout)
{
    double dPos = convertToEncoderUnits(chassis, distance);

    leftTopMotor.move_relative(dPos, speed);
    leftBottomMotor.move_relative(dPos, speed);
    rightTopMotor.move_relative(dPos, speed);
    rightBottomMotor.move_relative(dPos, speed);

    waitUntilMotorWithin(leftTopMotor, dPos, 15, timeout);
}

void moveIntakeAtSharedSpeed(int velocity)
{
    leftIntake.move_velocity(velocity);
    rightIntake.move_velocity(velocity);
}

void setChassisBrakeMode(pros::motor_brake_mode_e_t brakeMode)
{
    leftTopMotor.set_brake_mode(brakeMode);
    leftBottomMotor.set_brake_mode(brakeMode);
    rightTopMotor.set_brake_mode(brakeMode);
    rightBottomMotor.set_brake_mode(brakeMode);
}

void tareChassis()
{
    leftTopMotor.tare_position();
    leftBottomMotor.tare_position();
    rightTopMotor.tare_position();
    rightBottomMotor.tare_position();
}

void moveChassisDistance(std::shared_ptr<okapi::ChassisController> chassis,
    okapi::QLength leftDistance, okapi::QLength rightDistance, int velocity)
{
    double dPosLeft = convertToEncoderUnits(chassis, leftDistance);
    double dPosRight = convertToEncoderUnits(chassis, rightDistance);

    int leftVelocity = velocity;
    int rightVelocity = velocity;

    // Ensure motors get to the same place at the same time
    // If the left distance is greater than the right distance, drop the left speed
    double percentDifference = abs(dPosLeft - dPosRight) / ((dPosLeft + dPosRight) / 2.0);

    leftTopMotor.move_relative(dPosLeft, velocity);
    leftBottomMotor.move_relative(dPosLeft, velocity);
    rightTopMotor.move_relative(dPosRight, velocity);
    rightBottomMotor.move_relative(dPosRight, velocity);

    waitUntilMotorWithin(leftTopMotor, dPosLeft, 15, 10000);
}