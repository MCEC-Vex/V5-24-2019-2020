#ifndef MCEC_V5_TEXTDISPLAYSCREEN_H
#define MCEC_V5_TEXTDISPLAYSCREEN_H

#include "ScrollingScreen.h"

class TextDisplayScreen : public ScrollingScreen
{
private:
    std::string* strArr;
protected:
    void onExit() override;
    void cycleOption(int line, bool forwards) override;
    std::string getLine(int line, bool selected) override;
public:
    TextDisplayScreen(DisplayCore *core, std::string* strArr, int count);
};

#endif //MCEC_V5_TEXTDISPLAYSCREEN_H