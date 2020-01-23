#include "displayLib/DisplayCore.h"

DisplayCore::DisplayCore(DisplayController* displayController, pros::Controller controller) :
                                                    displayController(displayController), controller(controller)
{
    
}

void DisplayCore::pushScreen(DisplayScreen* screen)
{
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
    // "lastChecked" used to determine the polling interval of checkInput
    // this value is for determining the speed the joysticks should move the options
    unsigned long timeSince = pros::millis() - lastChecked;
    lastChecked = pros::millis();

    // Only can send one type of input per polling interval
    ScreenInputType input;
    // The amount of that input to send. Used by the trigger buttons to multiply the speed
    int count = 0;

    // The single-press up,down,left,right buttons
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
        // Get the "most significant" axis of the joystick based on which of (x, y) is larger
        // Add the two sides to combine them and support left and right-handed methods
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
        
        // Remove the input counter progress if there's no partial input
        if(significantY == 0 && significantX == 0)
        {
            lastTimedInput = 0;
        }

        // Add to the input counter
        bool horizontal = abs(significantX) > abs(significantY);
        lastTimedInput += abs(horizontal ? significantX : significantY) * timeSince;
        
        // Set input counts
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
            count *= 5;
        }
        if(controller.get_digital(DIGITAL_L2) || controller.get_digital(DIGITAL_R2))
        {
            count *= 20;
        }

        for(int i = 0; i < count; i++)
        {
            screens.top()->onInput(input);
        }
    }
}