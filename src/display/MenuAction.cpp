#include "displayLib/MenuAction.h"

MenuAction::MenuAction(std::function<void(DisplayCore*)> primary, std::string name, DisplayCore* displayCore) :
        primary(primary), name(name), displayCore(displayCore)
{

}

void MenuAction::trigger()
{
    primary(displayCore);
}

std::string MenuAction::getName()
{
    return name;
}