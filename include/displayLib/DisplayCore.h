#ifndef MCEC_V5_DISPLAYCORE_H
#define MCEC_V5_DISPLAYCORE_H

#define PROS_USE_SIMPLE_NAMES
#include <stack>
#include "api.h"
#include "DisplayScreen.h"
#include "DisplayController.h"

class DisplayScreen;

// A cutoff that the joystick must reach over time to trigger input
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

    /**
     * Get the Display Controller object
     * 
     * @return DisplayController* A pointer to the display controller
     */
    DisplayController* getDisplayController();

    /**
     * Pushes a screen to the top of the display stack and calls the "onPush" callback
     * 
     * @param screen The screen to push
     */
    void pushScreen(DisplayScreen* screen);

    /**
     * Get the screen currently at the top of the stack
     * 
     * @return DisplayScreen* 
     */
    DisplayScreen* getTopScreen();

    /**
     * Pops the screen on the top of the stack
     */
    void popScreen();

    /**
     * Called to check input and pass to the top screen on the stack
     */
    void checkInput();

    void waitForSize(int size);

    void waitForPop();

    /**
     * Blocks until the stack is empty
     */
    void waitForEmpty();
};

#endif //MCEC_V5_DISPLAYCORE_H