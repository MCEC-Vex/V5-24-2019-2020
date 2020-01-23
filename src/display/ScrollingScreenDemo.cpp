#include "displayLib/ScrollingScreenDemo.h"

ScrollingScreenDemo::ScrollingScreenDemo(DisplayCore *core) : ScrollingScreen(core, 100, false, true)
{
    for(int i = 0; i < 100; i++)
    {
        state[i] = 0;
    }
}

void ScrollingScreenDemo::cycleOption(int line, bool forwards)
{
    state[line] += forwards ? 1 : -1;
}

void ScrollingScreenDemo::onExit()
{

}

std::string ScrollingScreenDemo::getLine(int line, bool selected)
{
    std::string text = std::to_string(line);
    text.append("    ");
    if(selected)
    {
        text.append("{");
    }
    else
    {
        text.append(" ");
    }
    text.append(std::to_string(state[line]));
    if(selected)
    {
        text.append("}");
    }

    return text;
}