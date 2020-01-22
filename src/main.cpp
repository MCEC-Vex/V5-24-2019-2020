#include "main.h"

// Motor positions
//TODO document the process for obtaining these values
#define LEFT_ARM_LOW_SAFE 216
#define RIGHT_ARM_LOW_SAFE -211
#define TRAY_LOWEST 0
#define TRAY_HIGHEST -1980
#define TRAY_ARM_SAFE -750
#define ARM_TOWER_LOW_LEFT 1500
#define ARM_TOWER_HIGH_LEFT 1910

// Arm software limits for bot safety
#define ARMS_LOWEST 0
#define ARMS_HIGHEST 2500

// Control definitions
#define ARMS_UP DIGITAL_UP
#define ARMS_DOWN DIGITAL_DOWN

#define BACKWARDS_OUT DIGITAL_B
#define ROLLER_OUTTAKE DIGITAL_R2
#define ROLLER_INTAKE DIGITAL_R1

#define TRAY_OUT DIGITAL_L1
#define TRAY_IN DIGITAL_L2

#define ARM_MACRO DIGITAL_X

// Motors are numbered from left to right
pros::Motor leftTopMotor(1);
pros::Motor leftBottomMotor(2);
pros::Motor rightTopMotor(3);
pros::Motor rightBottomMotor(4);
pros::Motor centerWheel(15);

pros::Motor leftIntake(11);
pros::Motor rightIntake(12);

pros::Motor leftArmMotor(8);
pros::Motor rightArmMotor(9);

pros::Motor trayMotorBack(13);
pros::Motor trayMotorFront(14);

pros::ADIPort autoRedSmall(1);
pros::ADIPort autoRedBig(2);
pros::ADIPort autoBlueSmall(3);
pros::ADIPort autoBlueBig(4);
//pros::ADIPort autoSafe(5);
//pros::ADIPort autoTesting(6);
pros::ADIUltrasonic frontUltrasonic(7, 8);
pros::ADIUltrasonic backUltrasonic(5, 6);
okapi::MedianFilter<5> frontUltrasonicFilter;
okapi::MedianFilter<5> backUltrasonicFilter;

pros::Controller master(pros::E_CONTROLLER_MASTER);

DisplayController displayController(master);

/**
 * Task to run the controller display cycle
 */
void displayTimerTask(void* unused)
{
    while(true)
    {
        displayController.sendNext();

        pros::delay(50);
    }
}

auto chassis = okapi::ChassisControllerBuilder()
        // Left side is 1,2 right side is -3,-4 (negative indicates reversed)
        .withMotors({1, 2}, {-3, -4})
        .withDimensions(okapi::AbstractMotor::gearset::green, {{4_in, 12.5_in}, okapi::imev5GreenTPR})
        .build();

/**
 * Set the tray position to a particular point with specified speed
 *
 * @param trayPos The position to set the tray. With the current bot setup, this goes from 0 to negative as the tray goes out
 * @param speed The speed from 0-200 to move the tray
 */
void setTrayPosition(int trayPos, int speed)
{
    trayMotorFront.move_absolute(trayPos, speed);
    trayMotorBack.move_absolute(trayPos, speed);
}

/**
 * Used when moving the arms
 * Checks the arm position against a list of bounds to set the tray position accordingly
 */
#define TRAY_SHIFT_SPEED 200
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

/**
 * Flip the tray out by moving the arms and tray violently
 */
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

/**
 * Center the bot against the right wall
 */
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

/**
 * Autonomous to gather the four cubes on the far right and stack them in the right corner
 *
 * @param red True if the bot is on the red alliance, false if on blue
 */
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

    // Turn to face scoring zone
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

/**
 * Run auto for the sideways "L" of cubes near the right
 *
 * @param red True if the bot is on the red alliance, false if on blue
 */
void runAutoBig(bool red)
{
    //TODO fully document big auto process
    // 250 for arms
    int sign = red ? 1 : -1;

    //flipTray();
    pros::delay(1500);

    chassis->setMaxVelocity(25);

    //move forward and intake "2nd row"
    leftIntake.move_velocity(200);
    rightIntake.move_velocity(-200);
    chassis->moveDistance(1.75_ft);

    pros::delay(1000);

    //arm movement to intake the stacked cubes at end of "2nd row"
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->moveDistance(-0.2_ft);
    pros::delay(1000);
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
    chassis->moveDistance(-0.5_ft);

    // Move forward and intake cube stack... copied from runAutoSmall

    leftIntake.move_velocity(200);
    rightIntake.move_velocity(-200);
    chassis->moveDistance(2.5_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->moveDistance(-1.5_ft);

    // Turn to face scoring zone
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

/**
 * Sets up the motor brake modes
 */
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

void displayTesting()
{
    //displayController.setLine(0, "Testing Mode");
    displayController.clearLine(0);
    displayController.clearLine(1);
    displayController.clearLine(2);
    pros::delay(150);
    DisplayCore core(&displayController, master);
    ScrollingScreenDemo demo(&core);
    core.pushScreen(&demo);

    while(true)
    {
        core.checkInput();
        if(master.get_digital_new_press(DIGITAL_B))
        {
            break;
        }
        pros::delay(10);
    }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
    pros::lcd::initialize();
    setupMotors();
    pros::Task displayTask(displayTimerTask, (void*)"", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize()
{}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous()
{
    if(autoRedSmall.get_value())
    {
        runAutoSmall(true);
    }
    else if(autoBlueSmall.get_value())
    {
        runAutoSmall(false);
    }
    else if(autoRedBig.get_value())
    {
        runAutoBig(true);
    }
    else if(autoBlueBig.get_value())
    {
        runAutoBig(false);
    }
    /*else if(autoSafe.get_value())
    {
        chassis->setMaxVelocity(75);
        chassis->moveDistance(3_ft);
    }*/
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol()
{
    // Flags for setting 0 velocity
    bool trayWasMoving = false;
    bool armsWereMoving = false;

    // Arm macro flag
    int armMacroPos = 0;

    while(true)
    {
        // Arcade-style driving controls
        int forwardPower = master.get_analog(ANALOG_LEFT_Y);
        int turningPower = master.get_analog(ANALOG_RIGHT_X);

        // Make drive significantly slower when tray buttons are held
        if(master.get_digital(TRAY_OUT) || master.get_digital(TRAY_IN))
        {
            forwardPower /= 4;
            turningPower /= 4;
        }

        leftTopMotor.move(forwardPower + turningPower);
        leftBottomMotor.move(forwardPower + turningPower);
        rightTopMotor.move((forwardPower - turningPower) * -1);
        rightBottomMotor.move((forwardPower - turningPower) * -1);

        if(master.get_digital_new_press(DIGITAL_X))
        {
            //flipTray();
            //displayTesting();
            //shuffle(5_deg, 2_in, 20);
            centerBot(5, 0.2);
        }

        if(master.get_digital_new_press(DIGITAL_Y))
        {
            autonomous();
        }

        if(master.get_digital(TRAY_OUT))
        {
            if(trayMotorBack.get_position() <= TRAY_HIGHEST)
            {
                // Refuse to move the tray above its lowest safe point
                displayController.rumble(".");
                trayMotorBack.move_velocity(0);
                trayMotorFront.move_velocity(0);
            }
            else
            {
                int speed = -80;
                // Flick the tray when "A" is being held to deploy the tray
                if(master.get_digital(DIGITAL_A))
                {
                    speed = -200;
                }
                else if(trayMotorFront.get_position() < -900)
                {
                    //TODO clean up math
                    // Reduces tray speed as it reaches the apex
                    speed = -20 - ((60.0 / 1000.0) * (1000 - abs(trayMotorFront.get_position() + 900)));
                }

                trayMotorBack.move_velocity(speed);
                trayMotorFront.move_velocity(speed);

                trayWasMoving = true;
            }
        }
        else if(master.get_digital(TRAY_IN))
        {
            //TODO add failsafe in case tray position reporting fails
            //     For example: holding "a" bypasses tray soft restrictions
            if(trayMotorBack.get_position() >= TRAY_LOWEST)
            {
                // Refuse to move the tray below its lowest safe point
                displayController.rumble(".");

                trayMotorBack.move_velocity(0);
                trayMotorFront.move_velocity(0);
            }
            else
            {
                // Move the tray quickly if A is pressed, slowly if Y is pressed, and regular speed if neither is pressed
                //TODO clean up this logic
                trayMotorBack.move_velocity(master.get_digital(DIGITAL_A) ? 200 :
                                            master.get_digital(DIGITAL_Y) ? 50 : 80);
                trayMotorFront.move_velocity(master.get_digital(DIGITAL_A) ? 200 :
                                             master.get_digital(DIGITAL_Y) ? 50 : 80);
                trayWasMoving = true;
            }
        }
        else if(trayWasMoving)
        {
            trayMotorBack.move_velocity(0);
            trayMotorFront.move_velocity(0);
            trayWasMoving = false;
        }

        if(master.get_digital(ARMS_UP))
        {
            if(leftArmMotor.get_position() >= ARMS_HIGHEST)
            {
                // Refuse to move the tray above its lowest safe point
                displayController.rumble(".");
                leftArmMotor.move_velocity(0);
                rightArmMotor.move_velocity(0);
            }
            else
            {
                // Move the arms up
                // Slow speed is 80
                leftArmMotor.move_velocity(150);
                rightArmMotor.move_velocity(-150);

                armsWereMoving = true;
                checkTrayArmsPos();
            }
        }
        else if(master.get_digital(ARMS_DOWN))
        {
            if(leftArmMotor.get_position() <= ARMS_LOWEST)
            {
                // Refuse to move the tray above its lowest safe point
                displayController.rumble(".");
                leftArmMotor.move_velocity(0);
                rightArmMotor.move_velocity(0);
            }
            else
            {
                // Move the arms down
                leftArmMotor.move_velocity(-150);
                rightArmMotor.move_velocity(150);

                armsWereMoving = true;
                checkTrayArmsPos();
            }
        }
        else if(armsWereMoving)
        {
            armsWereMoving = false;
            leftArmMotor.move_velocity(0);
            rightArmMotor.move_velocity(0);
        }

        if(master.get_digital(ROLLER_OUTTAKE))
        {
            // Move intake out
            // Move slowly if A is pressed
            if(!master.get_digital(DIGITAL_A))
            {
                leftIntake.move(-127);
                rightIntake.move(127);
            }
            else
            {
                leftIntake.move_velocity(-100);
                rightIntake.move_velocity(100);
            }
        }
        else if(master.get_digital(ROLLER_INTAKE))
        {
            // Move intake in
            // Move slowly if A is pressed
            if(!master.get_digital(DIGITAL_A))
            {
                leftIntake.move(127);
                rightIntake.move(-127);
            }
            else
            {
                leftIntake.move_velocity(100);
                rightIntake.move_velocity(-100);
            }
        }
        else
        {
            leftIntake.move(0);
            rightIntake.move(0);
        }

        // Back up the robot while spinning the intake out
        if(master.get_digital(BACKWARDS_OUT))
        {
            leftTopMotor.move(-50);
            leftBottomMotor.move(-50);
            rightTopMotor.move(50);
            rightBottomMotor.move(50);

            leftIntake.move(-80);
            rightIntake.move(80);
        }

        if(master.get_digital(DIGITAL_LEFT))
        {
            centerWheel.move(127);
        }
        else if(master.get_digital(DIGITAL_RIGHT))
        {
            centerWheel.move(-127);
        }
        else
        {
            centerWheel.move(0);
        }

        // Print debugging data
        /*pros::lcd::print(1, "Left Y: %d", forwardPower);
        pros::lcd::print(2, "Right X: %d", turningPower);
        pros::lcd::print(3, "L-Voltage: %d", forwardPower + turningPower);
        pros::lcd::print(4, "R-Voltage: %d", forwardPower - turningPower);
        pros::lcd::print(5, "Left Arm Pos: %f", leftArmMotor.get_position());
        pros::lcd::print(6, "Right Arm Pos: %f", rightArmMotor.get_position());
        pros::lcd::print(7, "Tray Pos (b): %f", trayMotorBack.get_position());*/

        //pros::lcd::print(1, "Front: %d", frontUltrasonic.get());
        //pros::lcd::print(2, "Back: %d", backUltrasonic.get());

        displayController.setLine(1, "F: " + std::to_string(frontUltrasonicFilter.filter(frontUltrasonic.get_value())));
        displayController.setLine(2, "B: " + std::to_string(backUltrasonicFilter.filter(backUltrasonic.get_value())));

        pros::delay(10);
    }
}
