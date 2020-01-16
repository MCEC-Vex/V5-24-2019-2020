
#ifndef MCEC_V5_DISPLAYCONTROLLER_H
#define MCEC_V5_DISPLAYCONTROLLER_H

#include "api.h"

class DisplayController
{
private:
    pros::Controller controller;
    pros::Mutex mutex;
    int lastUpdatedLine = -2;
    std::string lines[3];
    bool modified[3];
    std::string rumbleLine;

public:
    DisplayController(pros::Controller controller);

    void clearLine(int line);
    void setLine(int line, std::string data);
    void rumble(std::string pattern);

    void sendNext();
};


#endif //MCEC_V5_DISPLAYCONTROLLER_H
