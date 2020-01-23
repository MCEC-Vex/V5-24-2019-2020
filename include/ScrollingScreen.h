#ifndef MCEC_V5_SCROLLINGSCREEN_H
#define MCEC_V5_SCROLLINGSCREEN_H

#include "DisplayScreen.h"

class ScrollingScreen : public DisplayScreen
{
private:
    int line = 0;
    int count;
    bool continuous;
    bool alwaysCenter;
    void refreshAll();
protected:
    virtual void onExit() = 0;
    virtual void cycleOption(int line, bool forwards) = 0;
    virtual std::string getLine(int line, bool selected) = 0;
public:
    ScrollingScreen(DisplayCore *core, int lineCount, bool isContinuous, bool alwaysCenter) :
            DisplayScreen(core), count(lineCount), continuous(isContinuous), alwaysCenter(alwaysCenter) {};
    void onInput(ScreenInputType type) override;
    void onPush() override;
    void onPop() override;
};

#endif //MCEC_V5_SCROLLINGSCREEN_H