#ifndef MCEC_V5_SCROLLINGSCREENDEMO_H
#define MCEC_V5_SCROLLINGSCREENDEMO_H

#include "ScrollingScreen.h"

class ScrollingScreenDemo : public ScrollingScreen
{
protected:
    void onExit() override;
    void cycleOption(int line, bool forwards) override;
    std::string getLine(int line, bool selected) override;
public:
    ScrollingScreenDemo(DisplayCore *core);
};

#endif //MCEC_V5_SCROLLINGSCREENDEMO_H