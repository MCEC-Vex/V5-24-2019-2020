#include "util.h"

void setupMotors()
{
    // Set motors to hold so they don't move when they're not supposed to
    leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    centerWheel.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    trayMotorBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    trayMotorFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    leftArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    // Set the intake motors to "coast" as no significant difference was found between "hold" and "coast"
    leftIntake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    rightIntake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
}

void flipTray()
{
    // Move the arms up and use the pre-made tray movement function to adjust the tray in response
    leftArmMotor.move_absolute(500, 200);
    rightArmMotor.move_absolute(-500, 200);
    for(int i = 0; i < 100; i++)
    {
        checkTrayArmsPos();
        pros::delay(10);
    }

    // Move the arms back down in the same way
    leftArmMotor.move_absolute(0, 200);
    rightArmMotor.move_absolute(0, 200);
    for(int i = 0; i < 100; i++)
    {
        checkTrayArmsPos();
        pros::delay(10);
    }
}

void checkTrayArmsPos()
{
    if(leftArmMotor.get_position() < 20)
    {
        return;
    }

    if(leftArmMotor.get_position() < 100)
    {
        setTrayPosition(0, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 200)
    {
        setTrayPosition(-250, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 350)
    {
        setTrayPosition(-600, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 1500)
    {
        setTrayPosition(-1000, TRAY_SHIFT_SPEED);
    }
}

void setTrayPosition(int trayPos, int speed)
{
    trayMotorFront.move_absolute(trayPos, speed);
    trayMotorBack.move_absolute(trayPos, speed);
}