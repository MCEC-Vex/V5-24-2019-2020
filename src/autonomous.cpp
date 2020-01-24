#include "autonomous.h"

void centerBot(int tolerance, float speedMod)
{
    /*int averagePos = (frontUltrasonicFilter.filter(frontUltrasonic.get_value())
                    + backUltrasonicFilter.filter(backUltrasonic.get_value())) / 2;*/
    int leftDeviance, rightDeviance, error;
    int integral = 0;

    do
    {
        //leftDeviance = abs(averagePos - frontUltrasonic.get_value);
        error = (frontUltrasonic.get_value() - backUltrasonic.get_value());
        integral += error;
        if(integral > 10)
        {
            integral = 10;
        }
        else if(integral < -10)
        {
            integral = -10;
        }

        int speed = (abs(error) * 0.2) + integral;
        if(error > 0)
        {
            // Turn clockwise if front is larger than back
            rightTopMotor.move_velocity(speed);
            rightBottomMotor.move_velocity(speed);

            leftTopMotor.move_velocity(speed);
            leftBottomMotor.move_velocity(speed);

            displayController.setLine(0, "Clockwise");
        }
        else
        {
            rightTopMotor.move_velocity(speed * -1);
            rightBottomMotor.move_velocity(speed * -1);

            leftTopMotor.move_velocity(speed * -1);
            leftBottomMotor.move_velocity(speed * -1);

            displayController.setLine(0, "Counter");
        }
        displayController.setLine(1, std::to_string(error));

        pros::delay(20);
    }
    while(abs(error) > tolerance);
}

void runAutoSmall(bool red)
{
    int sign = red ? 1 : -1;

    flipTray();
    pros::delay(1500);

    // Move forward and intake cube stack
    chassis->setMaxVelocity(25);
    leftIntake.move_velocity(100);
    rightIntake.move_velocity(-100);
    chassis->moveDistance(2.5_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->moveDistance(-1.5_ft);

    // Turn to face, then move towards scoring zone
    chassis->turnAngle(105_deg * sign);
    chassis->moveDistance(0.5_ft);

    // Slightly outtake cubes
    leftIntake.move_velocity(-50);
    rightIntake.move_velocity(50);
    pros::delay(400);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(-0);

    // Move the tray up
    trayMotorFront.move_absolute(TRAY_HIGHEST, 50);
    trayMotorBack.move_absolute(TRAY_HIGHEST, 50);
    pros::delay(3000);
    trayMotorFront.move_absolute(TRAY_HIGHEST, 0);
    trayMotorBack.move_absolute(TRAY_HIGHEST, 0);

    pros::delay(500);

    // "Bump" the robot forward a bit
    chassis->moveDistance(1_in);

    // Back the robot up
    chassis->moveDistance(-1.6_ft);

    // Move the tray back down
    trayMotorFront.move_absolute(0, 50);
    trayMotorBack.move_absolute(0, 50);

    /*if(autoTesting.get_value())
    {
        // Go back to starting pos
        chassis->turnAngle(-105_deg * sign);
        chassis->moveDistance(-0.5_ft);
    }*/
}

void runAutoBig(bool red)
{
    //TODO fully document big auto process
    // 250 for arms
    int sign = red ? 1 : -1;

    flipTray();
    pros::delay(1000);

    chassis->setMaxVelocity(35);

    //move forward and intake "2nd row"
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(-200);
    chassis->moveDistance(1.75_ft);


    pros::delay(500);

    //arm movement to intake the stacked cubes at end of "2nd row"
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->moveDistance(-0.2_ft);
    pros::delay(500);
    leftArmMotor.move_absolute(350, 100);
    rightArmMotor.move_absolute(-350, 100);
    trayMotorBack.move_absolute(-500, 100);
    trayMotorFront.move_absolute(-500, 100);
    pros::delay(1000);

    chassis->moveDistance(0.4_ft);
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(-200);
    chassis->moveDistance(0.5_ft);

    leftArmMotor.move_absolute(0, 200);
    rightArmMotor.move_absolute(0, 200);
    trayMotorBack.move_absolute(0, 100);
    trayMotorFront.move_absolute(0, 100);

    pros::delay(1500);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    chassis->moveDistance(-1.0_ft);
    chassis->turnAngle(-60_deg * sign);
    chassis->moveDistance(-2.1_ft);
    chassis->turnAngle(60_deg * sign);

    //backup against the wall
    chassis->moveDistance(-0.4_ft);

    // Move forward and intake cube stack... copied from runAutoSmall

    leftIntake.move_velocity(200);
    rightIntake.move_velocity(-200);
    chassis->moveDistance(2.5_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->moveDistance(-1.5_ft);

    // Turn to face, then move towards, scoring zone
    chassis->turnAngle(105_deg * sign);
    chassis->moveDistance(0.2_ft);

    // Slightly outtake cubes
    leftIntake.move_velocity(-50);
    rightIntake.move_velocity(50);
    pros::delay(200);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(-0);

    // Move the tray up
    trayMotorFront.move_absolute(TRAY_HIGHEST, 50);
    trayMotorBack.move_absolute(TRAY_HIGHEST, 50);
    pros::delay(3000);
    trayMotorFront.move_absolute(TRAY_HIGHEST, 0);
    trayMotorBack.move_absolute(TRAY_HIGHEST, 0);

    pros::delay(500);

    // "Bump" the robot forward a bit
    chassis->moveDistance(1_in);

    // Back the robot up
    chassis->moveDistance(-1.6_ft);

    // Move the tray back down
    trayMotorFront.move_absolute(0, 50);
    trayMotorBack.move_absolute(0, 50);
}
