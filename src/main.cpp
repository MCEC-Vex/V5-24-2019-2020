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

// Prototypes for hidden vex functions to bypass PROS bug
extern "C" int32_t vexGenericSerialReceive( uint32_t index, uint8_t *buffer, int32_t length );
extern "C" void vexGenericSerialEnable(  uint32_t index, uint32_t nu );
extern "C" void vexGenericSerialBaudrate(  uint32_t index, uint32_t rate );

uint8_t cobsBuffer[1024];
uint8_t cobsDecodedBuffer[1024];
size_t cobsBufferIndex;

float gyroY = 0.0;
unsigned long lastAntiTipPacket = 0;

#define SERIALPORT 19

void onPacketReceived(const uint8_t* buffer, size_t size)
{
    PacketHeader header;
    memcpy(&header, buffer, sizeof(PacketHeader));

    if(header.type == CLEAR)
    {
        ClearPacket clearPacket;
        memcpy(&clearPacket, buffer + sizeof(PacketHeader), sizeof(ClearPacket));

        pros::lcd::clear_line(clearPacket.line);
    }
    else if(header.type == PRINT_DEMO)
    {
        PrintDemoPacket demoPacket;
        memcpy(&demoPacket, buffer + sizeof(PacketHeader), sizeof(PrintDemoPacket));

        pros::lcd::print(demoPacket.line, "Demo is working!");
    }
    else if(header.type == ANTI_TIP)
    {
        AntiTipPacket antiTipPacket;
        memcpy(&antiTipPacket, buffer + sizeof(PacketHeader), sizeof(AntiTipPacket));

        gyroY = antiTipPacket.gyroY;
        lastAntiTipPacket = pros::millis();
    }
}

void checkSerialTask(void* unused)
{
    while(true)
    {
        
        // Buffer to store serial data
        uint8_t buffer[256];
        int len = 256;
        
        // Get serial data
        int32_t nRead = vexGenericSerialReceive(SERIALPORT - 1, buffer, len);
        
        for(int i = 0; i < nRead; i++)
        {
            uint8_t data = buffer[i];

            if(data == 0)
            {
                //cobs_decode_result result;

                size_t numDecoded = COBS::decode(cobsBuffer, cobsBufferIndex, cobsDecodedBuffer);
                onPacketReceived(cobsDecodedBuffer, numDecoded);

                //result = cobs_decode(cobsDecodedBuffer, 1024, cobsBuffer, cobsBufferIndex);
                //onPacketReceived(cobsDecodedBuffer, result.out_len);

                cobsBufferIndex = 0;
            }
            else
            {
                //TODO implement overflow checks
                cobsBuffer[cobsBufferIndex++] = data;
            }
        }

        if(nRead > 0)
        {
            pros::lcd::print(0, "Got data! l: %d, bi: %d", nRead, cobsBufferIndex);

            std::string data = "";
            std::string dataSwapped = "";

            std::stringstream ss;
            ss << std::hex;

            for (int i = 0; i < nRead; i++)
            {
                uint8_t digit = buffer[i];
                //uint8_t digit_swapped = swap_endian<uint8_t>(digit);
                // Get current char
                //char thisDigit = (char)buffer[i] + '0';
				//myStream << thisDigit;
				data += std::to_string(digit);
                ss << std::setw(2) << std::setfill('0') << (int) digit;

                if(i != nRead - 1)
                {
                    ss << " ";
                    data += ",";
                }
                //dataSwapped += std::to_string(digit_swapped) + ",";
            }

            pros::lcd::print(1, data.c_str());
            //pros::lcd::print(2, ss.str().c_str());
        }

        // Now parse the data
        /*if(nRead > 0)
        {
            
            // Stream to put the characters in
            std::string data = "";
            bool recordAngle = false;
            
            // Go through characters
            for (int i = 0; i < nRead; i++) {
                // Get current char
                char thisDigit = (char)buffer[i];
				//myStream << thisDigit;
				data += thisDigit;
            }
			pros::lcd::print(2, "Got data! Length is %d", nRead);
			pros::lcd::print(3, data.c_str());
        }*/
    
        // Delay to let serial data arrive
        pros::delay(1);
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

    // Start serial on desired port
    vexGenericSerialEnable(SERIALPORT - 1, 0);
    
    // Set BAUD rate
    vexGenericSerialBaudrate(SERIALPORT - 1, 115200);
    
    // Let VEX OS configure port
    pros::delay(10);
    pros::Task serialTask(checkSerialTask, (void*)"", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT);
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
    bool intakesWereMoving = false;

    // Arm macro flag
    int armMacroPos = 0;

    unsigned long lastSent = pros::millis();

    unsigned long lastJiggleTime = 0;
    bool jiggleForwards;

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
        /*if(antiTipActivated)
        {
            antiTipActivated = false;

            for(int i = 0; i < 127; i++)
            {
                leftTopMotor.move(-1 * i);
                leftBottomMotor.move(-1 * i);
                rightTopMotor.move(i);
                rightBottomMotor.move(i);

                pros::delay(2);
            }
            
            pros::delay(200);

            for(int i = 127; i > 0; i--)
            {
                int speed = i;

                leftTopMotor.move(-1 * speed);
                leftBottomMotor.move(-1 * speed);
                rightTopMotor.move(speed);
                rightBottomMotor.move(speed);
                
                pros::delay(2);
            }
            leftTopMotor.move(0);
            leftBottomMotor.move(0);
            rightTopMotor.move(0);
            rightBottomMotor.move(0);
        }*/

        int antiTipSpeedChange = 0;
        //int packetTime = 100 + (abs(gyroY) * 5);
        int packetTime = 500;

        if(lastAntiTipPacket != 0 && gyroY != 0)
        {
            if(pros::millis() - lastAntiTipPacket < packetTime)
            {
                //antiTipSpeedChange = -1 * ANTI_TIP_SCALE * abs(gyroY);
                gyroY = 127;
                antiTipSpeedChange = -1 * gyroY;
                displayController.setLine(0, std::to_string(antiTipSpeedChange));
            }
            else if(gyroY > 0)
            {
                gyroY -= 1.2;
                //antiTipSpeedChange = -1 * ANTI_TIP_SCALE * abs(gyroY);
                antiTipSpeedChange = -1 * gyroY;

                if(gyroY < 0)
                {
                    gyroY == 0;
                }
            }
        }

        if(!tankMode)
        {
            // Arcade-style driving controls
            int forwardPower = master.get_analog(ANALOG_LEFT_Y) + antiTipSpeedChange;
            int turningPower = master.get_analog(ANALOG_RIGHT_X);
            if(antiTipSpeedChange != 0)
            {
                turningPower = 0;
            }

            // Make drive significantly slower when tray buttons are held
            if(master.get_digital(TRAY_OUT) || master.get_digital(TRAY_IN))
            {
                forwardPower /= 4;
                turningPower /= 4;

                if(pros::millis() - lastJiggleTime > 300 && trayMotorBack.get_position() < (TRAY_HIGHEST + 300)
                    && forwardPower < 0)
                {
                    lastJiggleTime = pros::millis();

                    jiggleForwards = !jiggleForwards;

                    if(jiggleForwards)
                    {
                        leftIntake.move(80);
                        rightIntake.move(-80);
                    }
                    else
                    {
                        leftIntake.move(-80);
                        rightIntake.move(80);
                    }
                }
                
            }
            else if(pros::millis() - lastJiggleTime > 100)
            {
                leftIntake.move(0);
                rightIntake.move(0);
            }

            leftTopMotor.move(forwardPower + turningPower);
            leftBottomMotor.move(forwardPower + turningPower);
            rightTopMotor.move((forwardPower - turningPower) * -1);
            rightBottomMotor.move((forwardPower - turningPower) * -1);
        }
        else
        {
            int leftPower = master.get_analog(ANALOG_LEFT_Y) + antiTipSpeedChange;
            int rightPower = master.get_analog(ANALOG_RIGHT_Y) + antiTipSpeedChange;

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
                /*else if(trayMotorFront.get_position() < -2000)
                {
                    speed = -10;
                }*/
                else if(trayMotorFront.get_position() < -2250)
                {
                    //TODO clean up math
                    // Reduces tray speed as it reaches the apex
                    //speed = -20 - ((60.0 / (TRAY_HIGHEST - 2250)) *
                    //        ((TRAY_HIGHEST - 2250) - abs(trayMotorFront.get_position() + (TRAY_HIGHEST - 2250))));
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
            intakesWereMoving = true;
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
            intakesWereMoving = true;
        }
        else if(intakesWereMoving)
        {
            leftIntake.move(0);
            rightIntake.move(0);
            intakesWereMoving = false;
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
        /*if(pros::millis() - lastSent > 1500)
        {
            lastSent = pros::millis();

            displayController.setLine(0, "L: " + std::to_string(leftIntake.get_current_draw()));
            displayController.setLine(1, "R: " + std::to_string(rightIntake.get_current_draw()));
        }*/
        
        //displayController.setLine(1, std::to_string(master.get_analog(ANALOG_LEFT_Y)));
        //displayController.setLine(2, std::to_string(master.get_analog(ANALOG_LEFT_X)));

        pros::delay(10);
    }
}//2.13.20
