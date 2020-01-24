#include "displayLib/ScrollingScreen.h"

void ScrollingScreen::onPop()
{
    onExit();
}

void ScrollingScreen::setLineCount(int lineCount)
{
    count = lineCount;
}

void ScrollingScreen::onInput(ScreenInputType input)
{
    if(input == LEFT || input == RIGHT)
    {
        cycleOption(line, input == RIGHT);

        int updateLine = 1;
        if(!continuous)
        {
            if(line == 0)
            {
                updateLine = 0;
            }
            else if(line == count - 1)
            {
                updateLine = 2;
            }
        }

        core->getDisplayController()->setLine(updateLine, getLine(line, true));
        pros::lcd::print(updateLine + 2, getLine(line, true).c_str());
    }

    if(input == UP || input == DOWN)
    {
        if(!continuous && ((input == UP && line == 0) || (input == DOWN && line == count - 1)))
        {
            core->getDisplayController()->rumble(".");
            return;
        }

        int from = line;
        line += (input == DOWN) ? 1 : -1;

        if(continuous)
        {
            if(line < 0)
            {
                line = count - 1;
            }
            else if(line > count - 1)
            {
                line = 0;
            }
            refreshAll();
        }
        else if(!alwaysCenter)
        {
            if(from == 0)
            {
                core->getDisplayController()->setLine(0, getLine(0, false));
                core->getDisplayController()->setLine(1, getLine(1, true));
                pros::lcd::print(2, getLine(0, false).c_str());
                pros::lcd::print(3, getLine(1, true).c_str());
            }
            else if(from == count - 1 && count != 2)
            {
                core->getDisplayController()->setLine(1, getLine(count - 2, true));
                core->getDisplayController()->setLine(2, getLine(count - 1, false));
                pros::lcd::print(3, getLine(count - 2, true).c_str());
                pros::lcd::print(4, getLine(count - 1, false).c_str());
            }
            else
            {
                refreshAll();
            }
        }
        else
        {
            refreshAll();
        }
    }

    if(input == SELECT)
    {
        onSelect(line);
    }

    if(input == BACK)
    {
        core->popScreen();
    }
}

void ScrollingScreen::refreshAll()
{
    int current = line - 1;
    if(continuous)
    {
        if(current == -1)
        {
            current = count - 1;
        }
    }
    else if(!alwaysCenter)
    {
        if(line == 0)
        {
            current++;
        }
        else if(line == count - 1 && count > 2)
        {
            current--;
        }
    }

    for(int i = 0; i < 3; i++)
    {
        if(current < 0 || current >= count)
        {
            core->getDisplayController()->clearLine(i);
            pros::lcd::print(i + 2, "                ");
        }
        else
        {
            core->getDisplayController()->setLine(i, getLine(current, current == line));
            pros::lcd::print(i + 2, getLine(current, current == line).c_str());
        }

        current++;
        if(current == count && continuous)
        {
            current = 0;
        }
    }
}

void ScrollingScreen::onPush()
{
    // Render all visible lines
    refreshAll();
}