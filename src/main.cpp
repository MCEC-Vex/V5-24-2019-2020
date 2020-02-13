#include "main.h"

/**
 * Task to run the controller display cycle
 */
void displayTimerTask(void* unused)
{
    while(true)
    {
        displayController.sendNext();
        pros::delay(51);
    }
}

void displayTesting()
{
    //displayController.setLine(0, "Testing Mode");
    displayController.clearLine(0);
    displayController.clearLine(1);
    displayController.clearLine(2);
    
    openMainMenu(&displayController, master);
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
    //runAutoSmall(false);
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
        runAutoBigFast(true);
    }
    else if(autoBlueBig.get_value())
    {
        runAutoBigFast(false);
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
    bool antiTipTriggered = false;

    // Arm macro flag
    int armMacroPos = 0;

    unsigned long lastSent = pros::millis();

    while(true)
    {
        /*if(tipSensor.get_value())
        {
            leftTopMotor.move(-127);
            leftBottomMotor.move(-127);
            rightTopMotor.move(127);
            rightBottomMotor.move(127);

            antiTipTriggered = true;

            pros::delay(10);
            continue;
        }

        if(antiTipTriggered)
        {
            pros::delay(100);
            leftTopMotor.move(0);
            leftBottomMotor.move(0);
            rightTopMotor.move(0);
            rightBottomMotor.move(0);
            antiTipTriggered = false;
        }*/

        if(!tankMode)
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
        }
        else
        {
            int leftPower = master.get_analog(ANALOG_LEFT_Y);
            int rightPower = master.get_analog(ANALOG_RIGHT_Y);

            // Make drive significantly slower when tray buttons are held
            if(master.get_digital(TRAY_OUT) || master.get_digital(TRAY_IN))
            {
                leftPower /= 4;
                rightPower /= 4;
            }

            leftTopMotor.move(leftPower);
            leftBottomMotor.move(leftPower);
            rightTopMotor.move(rightPower * -1);
            rightBottomMotor.move(rightPower * -1);
        }

        if(master.get_digital_new_press(DIGITAL_X))
        {
            if(disableMenu.get_value())
            {
                if(master.get_digital(DIGITAL_LEFT) || master.get_digital(DIGITAL_RIGHT))
                {
                    displayTesting();
                }
                else
                {
                    displayController.rumble(".");
                }
            }
            else
            {
                displayTesting();
            }   
        }

        if(master.get_digital_new_press(DIGITAL_Y))
        {
            flipTray();
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
                    speed = -50;
                }
                else if(trayMotorFront.get_position() < -2000)
                {
                    speed = -10;
                }
                else if(trayMotorFront.get_position() < -900)
                {
                    //TODO clean up math
                    // Reduces tray speed as it reaches the apex
                    /*speed = -20 - ((60.0 / TRAY_HIGHEST - 900) *
                            ((TRAY_HIGHEST - 900) - abs(trayMotorFront.get_position() + 900)));*/
                    speed = -30;
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
                trayMotorBack.move_velocity(master.get_digital(DIGITAL_A) ? 50 : 200);
                trayMotorFront.move_velocity(master.get_digital(DIGITAL_A) ? 50 : 200);
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
                if(leftArmMotor.get_position() < 100 && trayMotorFront.get_position() > -60)
                {
                    setTrayPosition(-100, 200);
                }
                else
                {
                    leftArmMotor.move_velocity(ARMS_SPEED);
                    rightArmMotor.move_velocity(ARMS_SPEED * -1);
                }

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
                leftArmMotor.move_velocity(ARMS_SPEED * -1);
                rightArmMotor.move_velocity(ARMS_SPEED);

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
                //leftIntake.move(-127);
                //rightIntake.move(127);
                moveIntakeAtSharedSpeed(-200);
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
                /*leftIntake.move(127);
                rightIntake.move(-127);*/

                // Deny intake if arms are up
                if(leftArmMotor.get_position() > 750)
                {
                    moveIntakeAtSharedSpeed(-200);
                }
                else
                {
                    moveIntakeAtSharedSpeed(200);
                }
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

        /*if(master.get_digital(DIGITAL_LEFT))
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
        }*/

        // Print debugging data
        //pros::lcd::print(1, "Left Y: %d", forwardPower);
        //pros::lcd::print(2, "Right X: %d", turningPower);
        //pros::lcd::print(3, "L-Voltage: %d", forwardPower + turningPower);
        //pros::lcd::print(4, "R-Voltage: %d", forwardPower - turningPower);
        //pros::lcd::print(2, "Left tip: %f", tipMotorLeft.get_position());
        //pros::lcd::print(3, "Right tip: %f", tipMotorRight.get_position());

        if(autoRedSmall.get_value())
        {
            pros::lcd::print(4, "Auton red small");
            displayController.setLine(2, "Auton red small");
        }
        else if(autoBlueSmall.get_value())
        {
            pros::lcd::print(4, "Auton blue small");
            displayController.setLine(2, "Auton blue small");
        }
        else if(autoRedBig.get_value())
        {
            pros::lcd::print(4, "Auton red big");
            displayController.setLine(2, "Auton red big");
        }
        else if(autoBlueBig.get_value())
        {
            pros::lcd::print(4, "Auton blue big");
            displayController.setLine(2, "Auton blue big");
        }
        else
        {
            pros::lcd::print(4, "NO AUTON");
            displayController.setLine(2, "NO AUTON");
        }

        pros::lcd::print(5, "Left Arm Pos: %f", leftArmMotor.get_position());
        pros::lcd::print(6, "Right Arm Pos: %f", rightArmMotor.get_position());
        pros::lcd::print(7, "Tray Pos (b): %f", trayMotorBack.get_position());

        //pros::lcd::print(1, "Front: %d", frontUltrasonic.get());
        //pros::lcd::print(2, "Back: %d", backUltrasonic.get());

        //displayController.setLine(1, "F: " + std::to_string(frontUltrasonicFilter.filter(frontUltrasonic.get_value())));
        //displayController.setLine(2, "B: " + std::to_string(backUltrasonicFilter.filter(backUltrasonic.get_value())));
        //displayController.setLine(0, "R: " + std::to_string(rearUltrasonicFilter.filter(rearUltrasonic.get_value())));

        //displayController.setLine(0, "L: " + std::to_string(leftIntake.get_temperature()));
        //displayController.setLine(1, "R: " + std::to_string(rightIntake.get_temperature()));
        if(pros::millis() - lastSent > 1500)
        {
            lastSent = pros::millis();

            displayController.setLine(0, "L: " + std::to_string(leftIntake.get_current_draw()));
            displayController.setLine(1, "R: " + std::to_string(rightIntake.get_current_draw()));
        }
        
        //displayController.setLine(1, std::to_string(master.get_analog(ANALOG_LEFT_Y)));
        //displayController.setLine(2, std::to_string(master.get_analog(ANALOG_LEFT_X)));

        pros::delay(10);
    }
}//2.13.20
