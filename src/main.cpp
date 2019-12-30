#include "main.h"
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{
    static bool pressed = false;
    pressed = !pressed;
    if(pressed)
    {
        pros::lcd::set_text(2, "I was pressed!");
    }
    else
    {
        pros::lcd::clear_line(2);
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

    pros::lcd::register_btn1_cb(on_center_button);
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

// Control definitions
#define ARMS_UP DIGITAL_UP
#define ARMS_DOWN DIGITAL_DOWN

#define BACKWARDS_OUT DIGITAL_B
#define ROLLER_OUTTAKE DIGITAL_R2
#define ROLLER_INTAKE DIGITAL_R1

#define TRAY_OUT DIGITAL_L2
#define TRAY_IN DIGITAL_L1

void opcontrol()
{
    pros::Controller master(pros::E_CONTROLLER_MASTER);

    // Motors are numbered from left to right
    pros::Motor leftTopMotor(1);
    pros::Motor leftBottomMotor(2);
    pros::Motor rightTopMotor(3);
    pros::Motor rightBottomMotor(4);

    pros::Motor leftIntake(6);
    pros::Motor rightIntake(7);

    pros::Motor leftArmMotor(8);
    pros::Motor rightArmMotor(9);

    pros::Motor trayMotor(10);

    // Set drive motors to coast to avoid straining the connections
    leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    // Set other motors to hold so they don't move when they're not supposed to
    trayMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    leftIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightIntake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    leftArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightArmMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    while(true)
    {
        // Arcade-style driving controls
        int forwardPower = master.get_analog(ANALOG_LEFT_Y);
        int turningPower = master.get_analog(ANALOG_RIGHT_X);

        leftTopMotor.move(forwardPower + turningPower);
        leftBottomMotor.move(forwardPower + turningPower);
        rightTopMotor.move((forwardPower - turningPower) * -1);
        rightBottomMotor.move((forwardPower - turningPower) * -1);

        if(master.get_digital(TRAY_OUT))
        {
            // Move the tray quickly if A is pressed, slowly if Y is pressed, and regular speed if neither is pressed
            //TODO clean up this logic
            trayMotor.move_velocity(master.get_digital(DIGITAL_A) ? 200 :
                master.get_digital(DIGITAL_Y) ? 50 : 80);
        }
        else if(master.get_digital(TRAY_IN))
        {
            trayMotor.move_velocity(master.get_digital(DIGITAL_A) ? -200 :
                master.get_digital(DIGITAL_Y) ? -50 : -80);
        }
        else
        {
            trayMotor.move_velocity(0);
        }

        if(master.get_digital(ARMS_UP))
        {
            // Move the arms up
            leftArmMotor.move_velocity(50);
            rightArmMotor.move_velocity(-50);
        }
        else if(master.get_digital(ARMS_DOWN))
        {
            // Move the arms down
            leftArmMotor.move_velocity(-50);
            rightArmMotor.move_velocity(50);
        }
        else
        {
            leftArmMotor.move_velocity(0);
            rightArmMotor.move_velocity(0);
        }

        if(master.get_digital(ROLLER_OUTTAKE))
        {
            // Move intake out
            leftIntake.move(50);
            rightIntake.move(-50);
        }
        else if(master.get_digital(ROLLER_INTAKE))
        {
            // Move intake in
            leftIntake.move(-127);
            rightIntake.move(127);
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

            leftIntake.move(50);
            rightIntake.move(-50);
        }

        // Print debugging data
        pros::lcd::print(1, "Left Y: %d", forwardPower);
        pros::lcd::print(2, "Right X: %d", turningPower);
        pros::lcd::print(3, "L-Voltage: %d", forwardPower + turningPower);
        pros::lcd::print(4, "R-Voltage: %d", forwardPower - turningPower);
        pros::lcd::print(5, "Left Arm Pos: %f", leftArmMotor.get_position());
        pros::lcd::print(6, "Right Arm Pos: %f", rightArmMotor.get_position());
        pros::lcd::print(7, "Tray Pos: %f", trayMotor.get_position());

        pros::delay(10);
    }
}
