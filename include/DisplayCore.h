#ifndef MCEC_V5_DISPLAYCORE_H
#define MCEC_V5_DISPLAYCORE_H

#define PROS_USE_SIMPLE_NAMES
#include <stack>
#include "api.h"
#include "DisplayScreen.h"
#include "DisplayController.h"

class DisplayScreen;

class DisplayCore
{
private:
    std::stack<DisplayScreen*> screens;
    DisplayController* displayController;
    pros::Controller controller;
public:
    DisplayCore(DisplayController* displayController, pros::Controller controller);

    DisplayController* getDisplayController();
    void pushScreen(DisplayScreen* screen);
    DisplayScreen* getTopScreen();
    void popScreen();
    void checkInput();
};

#endif //MCEC_V5_DISPLAYCORE_H