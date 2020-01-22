#include "../include/DisplayCore.h"

DisplayCore::DisplayCore(DisplayController* displayController, pros::Controller controller) :
                                                    displayController(displayController), controller(controller)
{
    
}


void DisplayCore::pushScreen(DisplayScreen* screen)
{
    pros::lcd::print(1, "Pushed screen");
    screens.push(screen);
    screen->onPush();
}

void DisplayCore::popScreen()
{
    screens.top()->onPop();
    screens.pop();
}

DisplayScreen* DisplayCore::getTopScreen()
{
    return screens.top();
}

DisplayController* DisplayCore::getDisplayController()
{
    return displayController;
}

void DisplayCore::checkInput()
{
    unsigned long timeSince = pros::millis() - lastChecked;
    lastChecked = pros::millis();

    ScreenInputType input;
    int count = 0;

    if(controller.get_digital_new_press(DIGITAL_LEFT))
    {
        input = ScreenInputType::LEFT;
        count++;
    }
    else if(controller.get_digital_new_press(DIGITAL_RIGHT))
    {
        input = ScreenInputType::RIGHT;
        count++;
    }
    else if(controller.get_digital_new_press(DIGITAL_UP))
    {
        input = ScreenInputType::UP;
        count++;
    }
    else if(controller.get_digital_new_press(DIGITAL_DOWN))
    {
        input = ScreenInputType::DOWN;
        count++;
    }
    else
    {
        int significantX = controller.get_analog(ANALOG_LEFT_X) + controller.get_analog(ANALOG_RIGHT_X);
        int significantY = controller.get_analog(ANALOG_LEFT_Y) + controller.get_analog(ANALOG_RIGHT_Y);

        // Handle mild joystick drift
        if(abs(significantY) < 10)
        {
            significantY = 0;
        }
        if(abs(significantX) < 10)
        {
            significantX = 0;
        }
        
        if(significantY == 0 && significantX == 0)
        {
            lastTimedInput = 0;
        }

        bool horizontal = abs(significantX) > abs(significantY);
        lastTimedInput += abs(horizontal ? significantX : significantY) * timeSince;
        
        while(lastTimedInput > TIMED_INPUT_CUTOFF)
        {
            lastTimedInput -= TIMED_INPUT_CUTOFF;
            count++;

            if(horizontal)
            {
                input = (significantX > 0) ? ScreenInputType::RIGHT : ScreenInputType::LEFT;
            }
            else
            {
                input = (significantY > 0) ? ScreenInputType::UP : ScreenInputType::DOWN;
            }
        }
    }

    if(count > 0)
    {
        // Trigger buttons increase selection speed
        if(controller.get_digital(DIGITAL_L1) || controller.get_digital(DIGITAL_R1))
        {
            count += 4;
        }
        if(controller.get_digital(DIGITAL_L2) || controller.get_digital(DIGITAL_R2))
        {
            count += 19;
        }

        for(int i = 0; i < count; i++)
        {
            screens.top()->onInput(input);
        }
    }
}