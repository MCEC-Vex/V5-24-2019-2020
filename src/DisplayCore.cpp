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

    if(controller.get_digital_new_press(DIGITAL_LEFT))
    {
        screens.top()->onInput(ScreenInputType::LEFT);
    }
    else if(controller.get_digital_new_press(DIGITAL_RIGHT))
    {
        screens.top()->onInput(ScreenInputType::RIGHT);
    }
    else if(controller.get_digital_new_press(DIGITAL_UP))
    {
        screens.top()->onInput(ScreenInputType::UP);
    }
    else if(controller.get_digital_new_press(DIGITAL_DOWN))
    {
        screens.top()->onInput(ScreenInputType::DOWN);
    }

    int significantX = controller.get_analog(ANALOG_LEFT_X);
    int significantY = controller.get_analog(ANALOG_LEFT_Y);
    if(abs(significantX) > abs(significantY))
    {
        significantY = 0;
    }
    else
    {
        significantX = 0;
    }
    lastTimedInput += abs(significantX + significantY) * timeSince;
    
    int count = 0;
    while(lastTimedInput > TIMED_INPUT_CUTOFF)
    {
        lastTimedInput -= TIMED_INPUT_CUTOFF;
        count++;

        // Trigger buttons increase selection speed
        if(controller.get_digital(DIGITAL_L1))
        {
            count += 4;
        }
        if(controller.get_digital(DIGITAL_L2))
        {
            count += 19;
        }
    }

    for(int i = 0; i < count; i++)
    {
        if(significantY == 0)
        {
            screens.top()->onInput((significantX > 0) ? ScreenInputType::RIGHT : ScreenInputType::LEFT);
        }
        else
        {
            screens.top()->onInput((significantY > 0) ? ScreenInputType::UP : ScreenInputType::DOWN);
        }
    }
}