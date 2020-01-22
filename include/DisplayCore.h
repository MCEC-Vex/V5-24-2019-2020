#ifndef MCEC_V5_DISPLAYCORE_H
#define MCEC_V5_DISPLAYCORE_H

#define PROS_USE_SIMPLE_NAMES
#include <stack>
#include "api.h"
#include "DisplayScreen.h"
#include "DisplayController.h"

class DisplayScreen;

#define TIMED_INPUT_CUTOFF 20000

class DisplayCore
{
private:
    std::stack<DisplayScreen*> screens;
    DisplayController* displayController;
    pros::Controller controller;
    unsigned long lastChecked = 0;
    unsigned long lastTimedInput = 0;
public:
    DisplayCore(DisplayController* displayController, pros::Controller controller);

    DisplayController* getDisplayController();
    void pushScreen(DisplayScreen* screen);
    DisplayScreen* getTopScreen();
    void popScreen();
    void checkInput();
};

#endif //MCEC_V5_DISPLAYCORE_H