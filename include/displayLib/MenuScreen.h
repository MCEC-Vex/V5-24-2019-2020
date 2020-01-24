#ifndef MCEC_V5_MENUSCREEN_H
#define MCEC_V5_MENUSCREEN_H

#include "ScrollingScreen.h"
#include "MenuAction.h"

class MenuScreen : public ScrollingScreen
{
private:
    MenuAction* menuArr;
protected:
    void onExit() override;
    void onSelect(int line) override;
    void cycleOption(int line, bool forwards) override;
    std::string getLine(int line, bool selected) override;
public:
    MenuScreen(DisplayCore *core, MenuAction* menuArr, int count);
};

#endif //MCEC_V5_MENUSCREEN_H