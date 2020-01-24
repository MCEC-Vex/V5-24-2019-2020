#include "displayLib/MenuScreen.h"

MenuScreen::MenuScreen(DisplayCore *core, MenuAction* menuArr, int count) :
        ScrollingScreen(core, count, true, false), menuArr(menuArr)
{
    
}

void MenuScreen::onExit()
{

}

void MenuScreen::cycleOption(int line, bool forwards)
{

}

std::string MenuScreen::getLine(int line, bool selected)
{
    std::string name;
    if(selected)
    {
        name = ">";
    }
    else
    {
        name = " ";
    }
    name.append(menuArr[line].getName());
    return name;
}

void MenuScreen::onSelect(int line)
{
    menuArr[line].trigger();
}