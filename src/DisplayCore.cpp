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
}