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

void moveDistanceParallel(okapi::QLength distance, okapi::QLength distanceFromWall)
{
    //use ultrasonics to determine current location & angle
    double x, y, theta;
    y = (frontUltrasonic.get_value() + backUltrasonic.get_value()) / 2.0;
    x = rearUltrasonic.get_value();
    theta = atan2(getUltrasonicDifference(), 260.0);
    profileController->generatePath({
      {x * 1_mm, y * 1_mm, theta * 1_rad},  // Profile starting position, this will normally be (0, 0, 0)
      {distance, distanceFromWall, 0_deg}}, // The next point in the profile, 3 feet forward
      "A" // Profile name
    );
    profileController->setTarget("A");
    profileController->waitUntilSettled();
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
}

void runAutoBig(bool red)
{
    //TODO fully document big auto process
    int sign = red ? 1 : -1;

    flipTray();
    pros::delay(500);

    chassis->setMaxVelocity(35);

    displayController.setLine(0, "1st intake");

    //move forward and intake "2nd row"
    leftIntake.move_velocity(AUTON_BIG_INTAKE_SPEED);
    rightIntake.move_velocity(AUTON_BIG_INTAKE_SPEED * -1);

    chassis->moveDistance(2.9_ft);

    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    pros::delay(250);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    displayController.setLine(0, "Going back");
    chassis->moveDistance(-2.0_ft);
    displayController.setLine(0, "Turning -90");
    chassis->turnAngle(-90_deg * sign);
    displayController.setLine(0, "Going back more");
    chassis->moveDistance(-1.25_ft);

    displayController.setLine(0, "Turing fancy");
    double rightTurnDistance = convertToEncoderUnits(chassis, 1.5_ft);
    //rightTopMotor.tare_position();
    rightTopMotor.move_relative(rightTurnDistance, 50);
    rightBottomMotor.move_relative(rightTurnDistance, 50);
    waitUntilMotorWithin(rightTopMotor, rightTurnDistance * -1, 15, 5000);

    //chassis->turnAngle(85_deg * sign);

    //backup against the wall
    displayController.setLine(0, "Backup bump");
    chassis->moveDistance(-0.1_ft);

    // Move forward and intake cube stack... copied from runAutoSmall

    leftIntake.move_velocity(AUTON_BIG_INTAKE_SPEED);
    rightIntake.move_velocity(AUTON_BIG_INTAKE_SPEED * -1);

    displayController.setLine(0, "Second forward");
    chassis->moveDistance(2.5_ft);

    pros::delay(200);
    // Put slight voltage pressure on the cubes
    //leftIntake.move(20);
    //rightIntake.move(-20);
    displayController.setLine(0, "Back 1.5");
    chassis->moveDistance(-1.50_ft);

    // Turn to face, then move towards, scoring zone
    displayController.setLine(0, "Turn 115 deg");
    chassis->turnAngle(115_deg * sign);
    chassis->moveDistance(0.35_ft);

    // Slightly intake cubes
    leftIntake.move_velocity(AUTON_BIG_INTAKE_SPEED);
    rightIntake.move_velocity(AUTON_BIG_INTAKE_SPEED * -1);
    pros::delay(400);
    // Slightly outtake cubes
    leftIntake.move_velocity(-50);
    rightIntake.move_velocity(50);
    pros::delay(400);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);

    // Move the tray up
    moveTrayToHighest();

    pros::delay(500);

    // "Bump" the robot forward a bit
    chassis->setMaxVelocity(15);
    chassis->moveDistance(3_in);

    // Back the robot up
    chassis->moveDistance(-1.6_ft);

    // Move the tray back down
    trayMotorFront.move_absolute(0, 50);
    trayMotorBack.move_absolute(0, 50);
}
