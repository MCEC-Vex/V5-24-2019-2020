#include "autonomous.h"

void runAutoSmall(bool red)
{
    int sign = red ? 1 : -1;

    flipTray();
    pros::delay(1500);

    // Move forward and intake cube stack
    chassis->setMaxVelocity(25);
    leftIntake.move_velocity(100);
    rightIntake.move_velocity(100);
    chassis->moveDistance(2.3_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->moveDistance(-1.3_ft);

    // Turn to face, then move towards scoring zone
    chassis->turnAngle(105_deg * sign);
    chassis->moveDistance(0.5_ft);

    // Slightly outtake cubes
    leftIntake.move_velocity(-50);
    rightIntake.move_velocity(-50);
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
    trayMotorFront.move_absolute(TRAY_LOWEST, 50);
    trayMotorBack.move_absolute(TRAY_LOWEST, 50);
}

void runAutoBigFast(bool red)
{
    int sign = red ? 1 : -1;

    flipTray();
    pros::delay(200);

    // 200 up from 80
    chassis->setMaxVelocity(80);

    //move forward and intake "2nd row"
    leftIntake.move_velocity(AUTON_BIG_INTAKE_SPEED_FAST);
    rightIntake.move_velocity(AUTON_BIG_INTAKE_SPEED_FAST);

    chassis->moveDistance(1.7_ft);
    /*chassis->setMaxVelocity(20);
    leftIntake.move_velocity(AUTON_BIG_INTAKE_SPEED_FAST / 2);
    rightIntake.move_velocity(AUTON_BIG_INTAKE_SPEED_FAST / 2 * -1);

    chassis->moveDistance(1.2_ft);
    chassis->setMaxVelocity(80);*/

    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    pros::delay(250);

    chassis->moveDistance(-1.2_ft);
    chassis->turnAngle(-88_deg * sign);
    chassis->moveDistance(-1.3_ft);

    double turnDistance = convertToEncoderUnits(chassis, 1.32_ft);
    //rightTopMotor.tare_position();
    if(red)
    {
        // 120 up from 50
        rightTopMotor.move_relative(turnDistance, 120);
        rightBottomMotor.move_relative(turnDistance, 120);
        waitUntilMotorWithin(rightTopMotor, turnDistance, 10, 3000);
    }
    else
    {
        double turnDistance = convertToEncoderUnits(chassis, 1.55_ft);
        leftTopMotor.move_relative(turnDistance * -1, 120);
        leftBottomMotor.move_relative(turnDistance * -1, 120);
        waitUntilMotorWithin(leftTopMotor, turnDistance * -1, 10, 3000);
    }
    pros::delay(500);

    // Move forward and intake cube stack... copied from runAutoSmall

    leftIntake.move_velocity(AUTON_BIG_INTAKE_SPEED_FAST);
    rightIntake.move_velocity(AUTON_BIG_INTAKE_SPEED_FAST);

    chassis->setMaxVelocity(70);
    chassis->moveDistance(2.8_ft);

    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    pros::delay(200);
    // Put slight voltage pressure on the cubes
    //leftIntake.move(20);
    //rightIntake.move(-20);
    chassis->moveDistance(-1.6_ft);

    // Turn to face, then move towards, scoring zone
    chassis->turnAngle(110_deg * sign);
    /*chassis->setMaxVelocity(40);
    if(red)
    {
        
    }
    else
    {
        chassis->turnAngle(127.5_deg * sign);
    }*/
    pros::delay(200);
    
    moveMotors(chassis, 0.55_ft, 15, 3000);

    // Slightly intake cubes
    //leftIntake.move_velocity(AUTON_BIG_INTAKE_SPEED);
    //rightIntake.move_velocity(AUTON_BIG_INTAKE_SPEED * -1);
    pros::delay(400);
    // Slightly outtake cubes
    leftIntake.move_velocity(-40);
    rightIntake.move_velocity(-40);
    pros::delay(200);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    // Move the tray up
    moveTrayToHighest();

    pros::delay(500);

    // "Bump" the robot forward a bit
    chassis->setMaxVelocity(15);
    moveMotors(chassis, 4.0_in, 15, 3000);
    //chassis->moveDistance(3_in);
    pros::delay(500);

    //moveMotors(chassis, -6_in, 30, 6000);

    moveMotors(chassis, -8_in, 30, 6000);

    // Move the tray back down
    trayMotorFront.move_absolute(TRAY_LOWEST, 150);
    trayMotorBack.move_absolute(TRAY_LOWEST, 150);

    // Back the robot up
    chassis->setMaxVelocity(60);
    chassis->moveDistance(-1.6_ft);
}

void runSpicyAuton(bool red)
{
    int sign = red ? 1 : -1;

    //flipTray();
    //pros::delay(200);

    // Move forward to collect the cubes from the first row
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(200);
    chassis->setMaxVelocity(80);
    chassis->moveDistance(3.4_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->setMaxVelocity(200);

    // Move backwards in an "S" to get second row
    profileController->generatePath({
        {3.1_ft, 2.0_ft * sign, 0_deg},
        {0_ft, 0_ft, 0_deg}},
        "Row2Prep" // Profile name
    );
    profileController->setTarget("Row2Prep", true);
    profileController->waitUntilSettled();

    // Get second row of cubes
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(200);

    chassis->setMaxVelocity(80);
    chassis->moveDistance(1.85_ft);
    
    pros::delay(400);
    
    chassis->setMaxVelocity(20);
    chassis->moveDistance(-0.3_ft);
    chassis->setMaxVelocity(200);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    // Turn 90 degrees and go to the goal
    // Move up tray
    setTrayPosition(TRAY_HIGHEST / 3, 30);
    leftIntake.move_relative(-90, 20);
    rightIntake.move_relative(-90, 20);

    //chassis->setMaxVelocity(200);
    chassis->turnAngle(90_deg * sign);

    pros::delay(500);
    //chassis->setMaxVelocity(200);

    profileController->generatePath({
        {0_ft, 0_ft, 0_deg},
        {1.50_ft, -1.90_ft * sign, red ? -65_deg : 71_deg}},
        "GoToGoal"
    );
    profileController->setTarget("GoToGoal");
    profileController->waitUntilSettled();

    // Deposit cubes
    setTrayPosition((TRAY_HIGHEST / 3) * 2, 40);
    moveMotors(chassis, 0.25_ft, 25, 1000);
    moveTrayToHighest();
    moveMotors(chassis, 0.1_ft, 10, 1000);

    pros::delay(500);

    //leftIntake.move(-80);
    //rightIntake.move(-80);
    //pros::delay(50);
    leftTopMotor.move_relative(-1500, 80);
    leftBottomMotor.move_relative(-1500, 80);
    rightTopMotor.move_relative(-1500, 80);
    rightBottomMotor.move_relative(-1500, 80);

    pros::delay(3000);
    leftIntake.move(0);
    rightIntake.move(0);

    setTrayPosition(0, 50);
}

void runAsymmetryForwardTests(int iterations)
{
    // 19.5 in offset to the left over 10 iterations
    // 6 in forwards

    // Okay I give up (for now)

    chassis->setMaxVelocity(80);
    for(int i = 0; i < iterations; i++)
    {
        moveChassisDistance(chassis, 4_ft, 3.91875_ft, 80);
        //chassis->moveDistance(4_ft);
        pros::delay(1000);
        moveChassisDistance(chassis, -4_ft - (0.6_in), -3.91875_ft - (0.6_in), 80);
        //chassis->moveDistance(-4_ft);
        pros::delay(1000);

        displayController.setLine(0, std::to_string(i + 1));
    }
}

void runAsymmetryTurningTests(int iterations)
{
    chassis->setMaxVelocity(80);
    chassis->turnAngle(90_deg);
    pros::delay(1000);
    for(int i = 0; i < iterations; i++)
    {
        chassis->turnAngle(-180_deg);
        pros::delay(1000);
        chassis->turnAngle(180_deg);
        pros::delay(1000);
    }
    chassis->turnAngle(-90_deg);
}