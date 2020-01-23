#include "displayLib/TextDisplayScreen.h"

TextDisplayScreen::TextDisplayScreen(DisplayCore *core, std::string* strArr, int count) :
        ScrollingScreen(core, count, false, true), strArr(strArr)
{
    
}

void TextDisplayScreen::onExit()
{

}

void TextDisplayScreen::cycleOption(int line, bool forwards)
{

}

std::string TextDisplayScreen::getLine(int line, bool selected)
{
    return strArr[line];
}