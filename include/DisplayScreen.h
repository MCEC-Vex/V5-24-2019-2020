#ifndef MCEC_V5_DISPLAYSCREEN_H
#define MCEC_V5_DISPLAYSCREEN_H

#include "DisplayCore.h"
#include "ScreenInputType.h"

class DisplayCore;

class DisplayScreen
{
protected:
    DisplayCore *core;
public:
    DisplayScreen(DisplayCore *displayCore) : core(displayCore) {};
    virtual void onInput(ScreenInputType type) = 0;
    virtual void onPush() = 0;
    virtual void onPop() = 0;
};

#endif //MCEC_V5_DISPLAYSCREEN_H