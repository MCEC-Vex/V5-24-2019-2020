#ifndef MCEC_V5_MENU_H
#define MCEC_V5_MENU_H

#include "displayLib/DisplayCore.h"
#include "displayLib/MenuScreen.h"
#include "displayLib/TextDisplayScreen.h"
#include "displayLib/MenuAction.h"
#include "autonomous.h"

void openMainMenu(DisplayController* controller, pros::Controller master);
void openAutoMenu(DisplayCore* core);
void openTempMenu(DisplayCore* core);
void openPosMenu(DisplayCore* core);
void openSensorMenu(DisplayCore* core);

#endif //MCEC_V5_CONSTANTS_H