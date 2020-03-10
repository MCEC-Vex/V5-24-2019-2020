#include "autonomous.h"

void runMildAuton(bool red)
{
    int sign = red ? 1 : -1;

    flipTray();
    pros::delay(200);

    // Move forward to collect the cubes from the first row
    leftIntake.move_velocity(150);
    rightIntake.move_velocity(150);
    chassis->setMaxVelocity(60);
    chassis->moveDistance(3.0_ft);
    pros::delay(200);

    if(red)
    {
        leftTopMotor.move_relative(-220, 80);
        leftBottomMotor.move_relative(-220, 80);
    }
    else
    {
        rightTopMotor.move_relative(-310, 80);
        rightBottomMotor.move_relative(-310, 80);
    }
    
    pros::delay(600);
    chassis->moveDistance(red ? 1_ft : 1.1_ft);
    pros::delay(300);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->setMaxVelocity(200);

    // Move backwards in an "S" to get second row
    if(red)
    {
        profileController->generatePath({
            {3.5_ft, 1.65_ft, 23_deg},
            {0_ft, 0_ft, 0_deg}},
            "Row2Prep" // Profile name
        );
        profileController->setTarget("Row2Prep", true);
        profileController->waitUntilSettled();
        pros::delay(200);
    }
    else
    {
        profileController->generatePath({
            {3.3_ft, -1.72_ft, 32_deg},
            {0_ft, 0_ft, 0_deg}},
            "Row2Prep" // Profile name
        );
        profileController->setTarget("Row2Prep", true);
        profileController->waitUntilSettled();

        rightTopMotor.move_relative(40, 80);
        rightBottomMotor.move_relative(40, 80);
        pros::delay(400);
        rightTopMotor.move(0);
        rightBottomMotor.move(0);
    }

    chassis->setMaxVelocity(80);
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(200);
    chassis->moveDistance(red ? 1.81_ft : 1.71_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    
    pros::delay(400);
}

void runSpicyAuton(bool red)
{
    int sign = red ? 1 : -1;

    flipTray();
    pros::delay(200);

    // Move forward to collect the cubes from the first row
    leftIntake.move_velocity(150);
    rightIntake.move_velocity(150);
    chassis->setMaxVelocity(60);
    chassis->moveDistance(3.0_ft);
    pros::delay(200);

    // Get the cube in front of the first tower
    if(red)
    {
        leftTopMotor.move_relative(-220, 80);
        leftBottomMotor.move_relative(-220, 80);
    }
    else
    {
        rightTopMotor.move_relative(-310, 80);
        rightBottomMotor.move_relative(-310, 80);
    }
    
    pros::delay(600);
    chassis->moveDistance(red ? 1_ft : 1.1_ft);
    pros::delay(300);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->setMaxVelocity(200);

    // Move backwards in an "S" to get second row
    if(red)
    {
        profileController->generatePath({
            {3.5_ft, 1.65_ft, -23_deg},
            {0_ft, 0_ft, 0_deg}},
            "Row2Prep" // Profile name
        );
        profileController->setTarget("Row2Prep", true);
        profileController->waitUntilSettled();
        pros::delay(200);
    }
    else
    {
        profileController->generatePath({
            {3.3_ft, -1.72_ft, 32_deg},
            {0_ft, 0_ft, 0_deg}},
            "Row2Prep" // Profile name
        );
        profileController->setTarget("Row2Prep", true);
        profileController->waitUntilSettled();

        // The bot is very slightly at the wrong angle on blue
        // This ended up working better than trying to tweak the S-curve
        rightTopMotor.move_relative(40, 80);
        rightBottomMotor.move_relative(40, 80);
        pros::delay(400);
        rightTopMotor.move(0);
        rightBottomMotor.move(0);
    }

    // Get two cubes from the second row
    chassis->setMaxVelocity(80);
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(200);
    chassis->moveDistance(red ? 1.81_ft : 1.71_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    
    pros::delay(400);

    // Push the stack forwards so we have more room lol
    chassis->setMaxVelocity(10);
    deployAntiTip();
    chassis->moveDistance(0.5_ft);

    leftIntake.move_velocity(50);
    rightIntake.move_velocity(50);

    // Move to get the cube by the tower
    if(red)
    {
        leftTopMotor.move_relative(-1700, 40);
        leftBottomMotor.move_relative(-1700, 40);
    }
    else
    {
        rightTopMotor.move_relative(-1700, 40);
        rightBottomMotor.move_relative(-1700, 40);
    }
    pros::delay(600);
    
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    if(red)
    {
        rightTopMotor.move_relative(300, 60);
        rightBottomMotor.move_relative(300, 60);
        pros::delay(600);
        rightTopMotor.move_relative(-400, 30);
        rightBottomMotor.move_relative(-400, 30);
        pros::delay(2200);
    }
    else
    {
        leftTopMotor.move_relative(300, 60);
        leftBottomMotor.move_relative(300, 60);
        pros::delay(600);
        leftTopMotor.move_relative(-400, 30);
        leftBottomMotor.move_relative(-400, 30);
        pros::delay(2200);
    }

    // Back up to give room for the S-curve
    chassis->setMaxVelocity(60);
    chassis->moveDistance(-1.2_ft);
    chassis->setMaxVelocity(200);
    pros::delay(200);

    // Use S-curve to align the bot for the second tower cube
    profileControllerChonky->generatePath({
        {0_ft, 0_ft, 0_deg},
        {1.3_ft, red ? 0.5_ft : -0.65_ft, red ? -30_deg : 20_deg}},
        "Tower2"
    );
    profileControllerChonky->setTarget("Tower2");
    profileControllerChonky->waitUntilSettled();

    // Intake the second tower cube and back up
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(200);
    chassis->setMaxVelocity(40);
    chassis->moveDistance(1.1_ft);
    pros::delay(200);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    chassis->setMaxVelocity(60);
    chassis->moveDistance(-1.1_ft);
    
    // Go back to base
    leftIntake.move_velocity(20);
    rightIntake.move_velocity(20);
    chassis->setMaxVelocity(40);
    chassis->turnAngle(-177_deg * sign);
    chassis->moveDistance(red ? 1.75_ft : 1.68_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    // Align left/right side
    if(red)
    {
        leftTopMotor.move_relative(200, 40);
        leftBottomMotor.move_relative(200, 40);
    }
    else
    {
        rightTopMotor.move_relative(200, 40);
        rightBottomMotor.move_relative(200, 40);
    }
    pros::delay(500);

    // Move the bot forward and back slightly (may be problematic)
    moveMotors(chassis, 0.2_ft, 10, 700);
    moveMotors(chassis, -0.15_ft, 10, 700);

    // Slightly outtake cubes
    leftIntake.move_velocity(-20);
    rightIntake.move_velocity(-20);
    pros::delay(500);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    // Move the tray up and push cubes forward
    moveTrayToHighest();
    pros::delay(700);
    moveMotors(chassis, 0.29_ft, 10, 1000);
    pros::delay(700);

    // Back up the bot
    chassis->setMaxVelocity(40);
    chassis->moveDistance(-1_ft);

    retractAntiTip();
    trayMotorBack.move_absolute(0, 100);
    trayMotorFront.move_absolute(0, 100);
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