#ifndef MCEC_V5_MENUACTION_H
#define MCEC_V5_MENUACTION_H

#include <functional>
#include "DisplayCore.h"

class MenuAction
{
private:
    std::function<void(DisplayCore*)> primary;
    DisplayCore* displayCore;
    std::string name;
public:
    MenuAction(std::function<void(DisplayCore*)> primary, std::string name, DisplayCore* displayCore);
    void trigger();
    std::string getName();
};

#endif //MCEC_V5_MENUACTION_H