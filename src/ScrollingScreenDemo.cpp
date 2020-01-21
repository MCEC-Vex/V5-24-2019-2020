#include "../include/ScrollingScreenDemo.h"

ScrollingScreenDemo::ScrollingScreenDemo(DisplayCore *core) : ScrollingScreen(core, 100, true)
{

}

void ScrollingScreenDemo::cycleOption(int line, bool forwards)
{

}

void ScrollingScreenDemo::onExit()
{

}

std::string ScrollingScreenDemo::getLine(int line, bool selected)
{
    std::string text = std::to_string(line);
    if(selected)
    {
        text.append("*");
    }
    return text;
}