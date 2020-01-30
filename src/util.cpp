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
    leftIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    tipMotorLeft.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    tipMotorRight.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    tipMotorRight.set_reversed(true);
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
        setTrayPosition(-100, TRAY_SHIFT_SPEED);
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

void moveTrayToHighest()
{
    while(trayMotorFront.get_position() > TRAY_HIGHEST)
    {
        int speed = -50;
        if(trayMotorFront.get_position() < -900)
        {
            speed = -10 - ((40.0 / 1000.0) * (1000 - abs(trayMotorFront.get_position() + 900)));
        }

        trayMotorBack.move_velocity(speed);
        trayMotorFront.move_velocity(speed);
        pros::delay(5);
    }
    trayMotorBack.move_velocity(0);
    trayMotorFront.move_velocity(0);
}

int getUltrasonicDifference()
{
    //return frontUltrasonic.get_value() - backUltrasonic.get_value();
    return 0;
}

void moveAlignedWithWallOkapi(std::shared_ptr<okapi::ChassisController> chassis, okapi::QLength distance)
{
    const double newTarget = distance.convert(okapi::meter) *
            chassis->getChassisScales().straight *
            chassis->getGearsetRatioPair().ratio;

    leftTopMotor.tare_position();

    pros::delay(15);

    leftTopMotor.move_relative(newTarget, 50);
    leftBottomMotor.move_relative(newTarget, 50);

    rightTopMotor.move_relative(newTarget * -1, 50);
    rightBottomMotor.move_relative(newTarget * -1, 50);

    while(leftTopMotor.get_position() < newTarget - 5)
    {
        pros::delay(10);
    }
}

void moveAlignedWithWall(int distance)
{
  rightTopMotor.tare_position();
  int leftSpeed = 25;
  int rightSpeed = -25;
  leftTopMotor.move(leftSpeed);
  leftBottomMotor.move(leftSpeed);
  rightTopMotor.move(rightSpeed);
  rightBottomMotor.move(rightSpeed);

displayController.setLine(0, std::to_string(rightTopMotor.get_position()));
while (abs(rightTopMotor.get_position()) < distance)
{
  displayController.setLine(0, std::to_string(rightTopMotor.get_position()));
  displayController.setLine(1, std::to_string(leftSpeed));
  displayController.setLine(2, std::to_string(rightSpeed));
    displayController.setLine(0, std::to_string(rightTopMotor.get_position()));
      /*if(abs(frontUltrasonic.get_value() - backUltrasonic.get_value()) < 10){
        leftSpeed = 25;
        rightSpeed = -25;
        leftTopMotor.move(leftSpeed);
        leftBottomMotor.move(leftSpeed);
        rightTopMotor.move(rightSpeed);
        rightBottomMotor.move(rightSpeed);
        pros::delay(100);
      }
      else if(frontUltrasonic.get_value() > backUltrasonic.get_value()){
        leftSpeed ++;
        rightSpeed ++;
        leftTopMotor.move(leftSpeed);
        leftBottomMotor.move(leftSpeed);
        rightTopMotor.move(rightSpeed);
        rightBottomMotor.move(rightSpeed);
      }
      else if(frontUltrasonic.get_value() < backUltrasonic.get_value()){
        leftSpeed --;
        rightSpeed --;
        leftTopMotor.move(leftSpeed);
        leftBottomMotor.move(leftSpeed);
        rightTopMotor.move(rightSpeed);
        rightBottomMotor.move(rightSpeed);
      }*/
      if(rightTopMotor.get_position() > distance){
        leftSpeed = 0;
        rightSpeed = 0;
        leftTopMotor.move(leftSpeed);
        leftBottomMotor.move(leftSpeed);
        rightTopMotor.move(rightSpeed);
        rightBottomMotor.move(rightSpeed);
      }
        pros::delay(10);
    }
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

    unsigned long current = pros::millis();
    while(abs(abs(motor.get_position() - startPos) - value) > tolerance && (pros::millis() - current) < timeout)
    {
        pros::delay(10);
    }
}

void moveMotors(std::shared_ptr<okapi::ChassisController> chassis, okapi::QLength distance, int speed, int timeout)
{
    double dPos = convertToEncoderUnits(chassis, distance);

    leftTopMotor.move_relative(dPos, speed);
    leftBottomMotor.move_relative(dPos, speed);
    rightTopMotor.move_relative(dPos * -1, speed);
    rightBottomMotor.move_relative(dPos * -1, speed);

    waitUntilMotorWithin(leftTopMotor, dPos, 15, timeout);
}

void moveIntakeAtSharedSpeed(int velocity)
{
    /*if(velocity > 80 && leftIntake.get_actual_velocity() > 120 && rightIntake.get_actual_velocity() < -120)
    {
        int difference = leftIntake.get_actual_velocity() - (rightIntake.get_actual_velocity() * -1);

        if(abs(difference) > 90)
        {
            leftIntake.move_velocity(-100);
            rightIntake.move_velocity(100);
            pros::delay(200);
            leftIntake.move_velocity(velocity);
            rightIntake.move_velocity(velocity * -1);
            pros::delay(200);
        }
    }
    else
    {
        leftIntake.move(velocity);
        rightIntake.move(velocity * -1);
    }*/
    leftIntake.move_velocity(velocity);
    rightIntake.move_velocity(velocity * -1);
}