#include "main.h"

/**
 * Task to run the controller display cycle
 */
void displayTimerTask(void* unused)
{
    bool wasPressed = false;

    while(true)
    {
        displayController.sendNext();

        if(disableBrake.get_value())
        {
            if(!wasPressed)
            {
                leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                wasPressed = true;
            }
        }
        else
        {
            if(wasPressed)
            {
                leftTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
                leftBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
                rightTopMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
                rightBottomMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

                leftTopMotor.move_velocity(0);
                leftBottomMotor.move_velocity(0);
                rightTopMotor.move_velocity(0);
                rightBottomMotor.move_velocity(0);

                wasPressed = false;
            }
        }

        pros::delay(51);
    }
}

void displayTesting()
{
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
unsigned int antiTipTriggerCount = 1;
pros::Mutex antiTipMutex;
bool arduinoAntiTipDisabled = false;

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
        if(!arduinoAntiTipDisabled && trayMotorFront.get_position() > -700)
        {
            antiTipMutex.take(20);
            AntiTipPacket antiTipPacket;
            memcpy(&antiTipPacket, buffer + sizeof(PacketHeader), sizeof(AntiTipPacket));

            gyroY = antiTipPacket.gyroY;
            lastAntiTipPacket = pros::millis();
            antiTipMutex.give();
        }
    }
    else if(header.type == CONTROL)
    {
        ControlPacket controlPacket;
        memcpy(&controlPacket, buffer + sizeof(PacketHeader), sizeof(ControlPacket));

        abstractController.setValue(controlPacket.input, controlPacket.value);
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
                // If the data ends in a 0 byte, this indicates the end of the packet
                // Decode the COBS packet into another buffer and call the packet callback
                size_t numDecoded = COBS::decode(cobsBuffer, cobsBufferIndex, cobsDecodedBuffer);
                onPacketReceived(cobsDecodedBuffer, numDecoded);
                cobsBufferIndex = 0;
            }
            else
            {
                // Add the data to the cobs buffer to be decoded at the end of the packet
                //TODO implement overflow checks
                cobsBuffer[cobsBufferIndex++] = data;
            }
        }

        // Print debugging data
        if(nRead > 0)
        {
            pros::lcd::print(0, "Got data! l: %d, bi: %d", nRead, cobsBufferIndex);

            std::string data = "";
            std::string dataSwapped = "";

            std::stringstream ss;
            ss << std::hex;

            for(int i = 0; i < nRead; i++)
            {
                uint8_t digit = buffer[i];
				data += std::to_string(digit);
                ss << std::setw(2) << std::setfill('0') << (int) digit;

                if(i != nRead - 1)
                {
                    ss << " ";
                    data += ",";
                }
            }

            pros::lcd::print(1, data.c_str());
        }
    
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
    //pros::Task disableBrakeTask(disableBrakeTask, (void*)"", TASK_PRIORITY_MIN, TASK_STACK_DEPTH_DEFAULT);

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
        runSpicyAuton(true);
    }
    else if(autoBlueBig.get_value())
    {
        runSpicyAuton(false);
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

    while(true)
    {
        abstractController.checkController();

        int antiTipSpeedChange = 0;

        // Check if there's a new anti-tip packet for us to cry about
        antiTipMutex.take(5);
        if(lastAntiTipPacket != 0 && gyroY != 0)
        {
            if(pros::millis() - lastAntiTipPacket < 500)
            {
                gyroY = 127;
                antiTipSpeedChange = -1 * gyroY;

                displayController.setLine(0, "Antitip: " + std::to_string(antiTipTriggerCount));
            }
            else if(gyroY > 0)
            {
                gyroY -= 1.2;
                antiTipSpeedChange = -1 * gyroY;

                if(gyroY <= 0)
                {
                    gyroY == 0;
                    antiTipTriggerCount++;
                }
            }
        }
        antiTipMutex.give();

        if(!tankMode)
        {
            // Arcade-style driving controls
            int forwardPower = abstractController.getAnalog(FORWARD_POWER) + antiTipSpeedChange;
            int turningPower = abstractController.getAnalog(TURNING_POWER);
            if(antiTipSpeedChange != 0)
            {
                turningPower = 0;
            }

            // Make drive significantly slower when tray buttons are held
            if(abstractController.getDigital(TRAY_OUT) || abstractController.getDigital(TRAY_IN))
            {
                forwardPower /= 4;
                turningPower /= 4;                
            }

            leftTopMotor.move(forwardPower + turningPower);
            leftBottomMotor.move(forwardPower + turningPower);
            rightTopMotor.move(forwardPower - turningPower);
            rightBottomMotor.move(forwardPower - turningPower);
        }
        else
        {
            int leftPower = abstractController.getAnalog(STICK_LEFT_Y) + antiTipSpeedChange;
            int rightPower = abstractController.getAnalog(STICK_RIGHT_Y) + antiTipSpeedChange;

            // Make drive significantly slower when tray buttons are held
            if(abstractController.getDigital(TRAY_OUT) || abstractController.getDigital(TRAY_IN))
            {
                leftPower /= 4;
                rightPower /= 4;
            }

            leftTopMotor.move(leftPower);
            leftBottomMotor.move(leftPower);
            rightTopMotor.move(rightPower);
            rightBottomMotor.move(rightPower);
        }

        if(abstractController.getDigitalNewPress(BUTTON_X))
        {
            if(disableMenu.get_value())
            {
                // During competition mode, only open the menu if either the right or left buttons are being held
                if(abstractController.getDigital(BUTTON_LEFT) || abstractController.getDigital(BUTTON_RIGHT))
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

        if(abstractController.getDigitalNewPress(BUTTON_Y))
        {
            // If left/right is being pressed at the same time, flip the tray (for driver skills)
            // Otherwise, toggle anti-tip
            if(abstractController.getDigital(BUTTON_LEFT) || abstractController.getDigital(BUTTON_RIGHT))
            {
                flipTray();
            }
            else
            {
                antiTipEnabled = !antiTipEnabled;
                if(antiTipEnabled)
                {
                    // If "A" is being held at the same time, go full power on the antitip motors
                    // *Just* in case there's a situation where we need the power
                    if(abstractController.getDigital(BUTTON_A))
                    {
                        leftAntiTip.move(127);
                        rightAntiTip.move(127);
                        displayController.setLine(1, "BEEF OUT");
                    }
                    else
                    {
                        deployAntiTip();
                        displayController.setLine(1, "Antitip out");
                    }
                }
                else
                {
                    retractAntiTip();
                    displayController.setLine(1, "Antitip in");
                }
            }
        }

        if(antiTipEnabled == false && (leftAntiTip.get_position() < 50 || rightAntiTip.get_position() < 50))
        {
            leftAntiTip.move(leftAntiTip.get_position() < 50 ? -10 : -25);
            rightAntiTip.move(rightAntiTip.get_position() < 50 ? -10 : -25);

            if(leftAntiTip.get_position() < -5)
            {
                leftAntiTip.tare_position();
            }
            if(rightAntiTip.get_position() < -5)
            {
                rightAntiTip.tare_position();
            }
        }

        if(abstractController.getDigital(TRAY_OUT))
        {
            if(trayMotorBack.get_position() >= TRAY_HIGHEST)
            {
                // Refuse to move the tray above its lowest safe point
                displayController.rumble(".");
                trayMotorBack.move_velocity(0);
                trayMotorFront.move_velocity(0);
            }
            else
            {
                int speed = 80;
                // Flick the tray when "A" is being held to deploy the tray
                if(abstractController.getDigital(BUTTON_A))
                {
                    speed = 50;
                }
                else if(trayMotorFront.get_position() > (TRAY_HIGHEST / 2))
                {
                    // Reduces tray speed as it reaches the apex
                    // Calculates "segments" from the current position to the max position
                    // Then, divide 60 by the number of segments and multiply that by an
                    // equation that is (number of segments) at minimum and 0 at maximum
                    float segments = TRAY_HIGHEST / 2;
                    speed = 20 + ((60.0 / segments) * (segments - abs(trayMotorFront.get_position() - (TRAY_HIGHEST / 2))));
                }

                trayMotorBack.move_velocity(speed);
                trayMotorFront.move_velocity(speed);

                //leftArmMotor.move(-100);
                //rightArmMotor.move(-100);

                trayWasMoving = true;
            }
        }
        else if(abstractController.getDigital(TRAY_IN))
        {
            //TODO add failsafe in case tray position reporting fails
            //     For example: holding "a" bypasses tray soft restrictions
            if(trayMotorBack.get_position() <= TRAY_LOWEST)
            {
                // Refuse to move the tray below its lowest safe point
                displayController.rumble(".");

                trayMotorBack.move_velocity(0);
                trayMotorFront.move_velocity(0);
            }
            else
            {
                if(trayMotorFront.get_position() < (TRAY_HIGHEST / 2))
                {
                    trayWasMoving = false;
                    setTrayPosition(0, 200);
                }
                else
                {
                    // Move the tray quickly if A is pressed and regular speed if not
                    trayMotorBack.move_velocity(abstractController.getDigital(BUTTON_A) ? -50 : -200);
                    trayMotorFront.move_velocity(abstractController.getDigital(BUTTON_A) ? -50 : -200);
                    trayWasMoving = true;
                }
            }
        }
        else if(trayWasMoving)
        {
            trayMotorBack.move_velocity(0);
            trayMotorFront.move_velocity(0);

            leftArmMotor.move(0);
            rightArmMotor.move(0);

            trayWasMoving = false;
        }

        if(abstractController.getDigital(ARMS_UP))
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
                if(leftArmMotor.get_position() < 100 && trayMotorFront.get_position() < 60)
                {
                    setTrayPosition(100, 200);
                }
                else
                {
                    leftArmMotor.move_velocity(ARMS_SPEED);
                    rightArmMotor.move_velocity(ARMS_SPEED);
                }

                armsWereMoving = true;
                checkTrayArmsPos();
            }
        }
        else if(abstractController.getDigital(ARMS_DOWN))
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
                rightArmMotor.move_velocity(ARMS_SPEED * -1);

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

        if(abstractController.getDigital(ROLLER_OUTTAKE))
        {
            // Move intake out
            // Move slowly if A is pressed
            if(abstractController.getDigital(BUTTON_A))
            {
                leftIntake.move_velocity(-100);
                rightIntake.move_velocity(-100);
            }
            else
            {
                leftIntake.move_velocity(-200);
                rightIntake.move_velocity(-200);
            }
            intakesWereMoving = true;
        }
        else if(abstractController.getDigital(ROLLER_INTAKE))
        {
            // Move intake in
            // Move slowly if A is pressed
            if(abstractController.getDigital(BUTTON_A))
            {
                leftIntake.move_velocity(100);
                rightIntake.move_velocity(100);
            }
            else
            {
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
            intakesWereMoving = true;
        }
        else if(intakesWereMoving)
        {
            leftIntake.move(0);
            rightIntake.move(0);
            intakesWereMoving = false;
        }

        // Back up the robot while spinning the intake out
        if(abstractController.getDigital(BACKWARDS_OUT))
        {
            leftTopMotor.move(-50);
            leftBottomMotor.move(-50);
            rightTopMotor.move(-50);
            rightBottomMotor.move(-50);

            leftIntake.move(-80);
            rightIntake.move(-80);
        }

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
            pros::lcd::print(4, "Auton red spicy");
            displayController.setLine(2, "Auton red spicy");
        }
        else if(autoBlueBig.get_value())
        {
            pros::lcd::print(4, "Auton blue spicy");
            displayController.setLine(2, "Auton blue spicy");
        }
        else
        {
            pros::lcd::print(4, "NO AUTON");
            displayController.setLine(2, "NO AUTON");
        }

        pros::delay(10);
    }
}
