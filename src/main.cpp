#include "main.h"
#include "DisplayController.h"

using namespace okapi::literals;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
    pros::lcd::initialize();
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
{}

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

// Motor positions
//TODO fix arm motors (went from regular to torque)
//TODO document the process for obtaining these values
#define LEFT_ARM_LOW_SAFE 216
#define RIGHT_ARM_LOW_SAFE -211
#define TRAY_LOWEST 0
//#define TRAY_HIGHEST -2270
#define TRAY_HIGHEST -1980
#define TRAY_ARM_SAFE -750
#define ARM_TOWER_LOW_LEFT 694
#define ARM_TOWER_LOW_RIGHT -668
#define ARM_TOWER_HIGH_LEFT 938
#define ARM_TOWER_HIGH_RIGHT -948

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

#define LINE_FLAG_LEFT true
#define LINE_FLAG_RIGHT false

pros::Motor leftTopMotor(1);
pros::Motor leftBottomMotor(2);
pros::Motor rightTopMotor(3);
pros::Motor rightBottomMotor(4);

pros::Motor leftIntake(6);
pros::Motor rightIntake(7);

pros::Motor leftArmMotor(8);
pros::Motor rightArmMotor(9);

pros::Motor trayMotorBack(11);
pros::Motor trayMotorFront(12);

pros::Controller master(pros::E_CONTROLLER_MASTER);

DisplayController displayController(master);

void displayTimerTask(void* unused)
{
    while(true)
    {
        displayController.sendNext();

        pros::delay(50);
    }
}

int getSimpleRandomInt()
{
    return (random() * 10);
}

auto chassis = okapi::ChassisControllerBuilder()
        // Left side is 1,2 right side is -3,-4 (negative indicates reversed)
        .withMotors({1, 2}, {-3, -4})
        .withDimensions(okapi::AbstractMotor::gearset::green, {{4_in, 12.5_in}, okapi::imev5GreenTPR})
        .build();

// Debugging string
std::string topLine = "";

void runAuto()
{
    // Move forward and intake cube stack
    chassis->setMaxVelocity(25);
    leftIntake.move_velocity(100);
    rightIntake.move_velocity(-100);
    chassis->moveDistance(2.5_ft);
    leftIntake.move_velocity(0);
    rightIntake.move_velocity(0);
    chassis->moveDistance(-1.5_ft);

    // Turn to face scoring zone
    chassis->turnAngle(105_deg);
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

    // Back the robot up
    chassis->moveDistance(-2_ft);
}

void setupMotors()
{
    // Set motors to hold so they don't move when they're not supposed to
    leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    trayMotorBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    trayMotorFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    leftIntake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    rightIntake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    leftArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

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
    if(leftArmMotor.get_position() < 100)
    {
        displayController.setLine(1, "Arm pos 0");
        setTrayPosition(0, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 200)
    {
        displayController.setLine(1, "Arm pos 1");
        setTrayPosition(-250, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 350)
    {
        displayController.setLine(1, "Arm pos 2");
        setTrayPosition(-600, TRAY_SHIFT_SPEED);
    }
    else if(leftArmMotor.get_position() < 1500)
    {
        displayController.setLine(1, "Arm pos 3");
        setTrayPosition(-1000, TRAY_SHIFT_SPEED);
    }
}

void opcontrol()
{
    setupMotors();
    pros::Task displayTask(displayTimerTask, (void*)"", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT);
    // Motors are numbered from left to right


    // Flags for setting 0 velocity
    bool trayWasMoving = false;
    bool armsWereMoving = false;
    bool intakeWasMoving = false;

    // Arm macro flag
    int armMacroPos = 0;

    // Display timing flag
    int lastDisplay = 0;
    bool lineFlag = LINE_FLAG_LEFT;
    int lastRumble = 0;

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

        if(master.get_digital_new_press(DIGITAL_Y))
        {
            runAuto();
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
                    // 1980
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
                leftArmMotor.move_velocity(80);
                rightArmMotor.move_velocity(-80);

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
                leftArmMotor.move_velocity(-50);
                rightArmMotor.move_velocity(50);
                checkTrayArmsPos();

                armsWereMoving = true;
            }
            /*if(leftArmMotor.get_position() < LEFT_ARM_LOW_SAFE && rightArmMotor.get_position() > RIGHT_ARM_LOW_SAFE)
            {
                trayMotorFront.move_absolute(TRAY_LOWEST, 200);
                trayMotorBack.move_absolute(TRAY_LOWEST, 200);
            }*/
        }
        else if(armsWereMoving)
        {
            armsWereMoving = false;
            leftArmMotor.move_velocity(0);
            rightArmMotor.move_velocity(0);
        }

        // Arm macros
        if(master.get_digital_new_press(ARM_MACRO))
        {
            if(armMacroPos == 0)
            {
                armMacroPos = 1;
                displayController.setLine(0, "Low Tower");
                trayMotorFront.move_absolute(TRAY_ARM_SAFE, 200);
                trayMotorBack.move_absolute(TRAY_ARM_SAFE, 200);

                leftArmMotor.move_absolute(ARM_TOWER_LOW_LEFT, 50);
                rightArmMotor.move_absolute(-1 * ARM_TOWER_LOW_LEFT, 50);
            }
            else if(armMacroPos == 1)
            {
                armMacroPos = 2;
                displayController.setLine(0, "High Tower");
                trayMotorFront.move_absolute(TRAY_ARM_SAFE, 200);
                trayMotorBack.move_absolute(TRAY_ARM_SAFE, 200);

                leftArmMotor.move_absolute(ARM_TOWER_HIGH_LEFT, 50);
                rightArmMotor.move_absolute(-1 * ARM_TOWER_HIGH_LEFT, 50);
            }
            else
            {
                armMacroPos = 0;
                displayController.setLine(0, "No Tower");

                rightArmMotor.move_absolute(34, 80);
                leftArmMotor.move_absolute(-34, 80);

                pros::delay(200);
                trayMotorFront.move_absolute(TRAY_LOWEST, 20);
                trayMotorBack.move_absolute(TRAY_LOWEST, 20);
            }
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
            //int speed = () ? 100 :200;
            //leftIntake.move_velocity(speed);
            //rightIntake.move_velocity(speed * -1);
            intakeWasMoving = true;
        }
        else if(master.get_digital(ROLLER_INTAKE))
        {
            // Move intake in
            // Move slowly if A is pressed
            /*int speed = (master.get_digital(DIGITAL_A)) ? 100 : 200;
            leftIntake.move_velocity(speed * -1);
            rightIntake.move_velocity(speed);*/

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

            intakeWasMoving = true;
        }
        else
        {
            leftIntake.move(0);
            rightIntake.move(0);
            intakeWasMoving = false;
        }

        // Back up the robot while spinning the intake out
        if(master.get_digital(BACKWARDS_OUT))
        {
            leftTopMotor.move(-50);
            leftBottomMotor.move(-50);
            rightTopMotor.move(50);
            rightBottomMotor.move(50);

            leftIntake.move(-50);
            rightIntake.move(50);
        }

        // Print debugging data
        pros::lcd::print(1, "Left Y: %d", forwardPower);
        pros::lcd::print(2, "Right X: %d", turningPower);
        pros::lcd::print(3, "L-Voltage: %d", forwardPower + turningPower);
        pros::lcd::print(4, "R-Voltage: %d", forwardPower - turningPower);
        pros::lcd::print(5, "Left Arm Pos: %f", leftArmMotor.get_position());
        pros::lcd::print(6, "Right Arm Pos: %f", rightArmMotor.get_position());
        pros::lcd::print(7, "Tray Pos (b): %f", trayMotorBack.get_position());

        pros::delay(10);
    }
}
