#ifndef MCEC_V5_MENU_H
#define MCEC_V5_MENU_H

#include "displayLib/DisplayCore.h"
#include "displayLib/MenuScreen.h"
#include "displayLib/TextDisplayScreen.h"
#include "displayLib/MenuAction.h"
#include "autonomous.h"

/**
 * Opens the main menu as a gateway to other menus
 * Note: This is a blocking function!
 * 
 * @param controller The DisplayController object to use for screen output
 * @param master The PROS Controller object for button input
 */
void openMainMenu(DisplayController* controller, pros::Controller master);

void openAutoMenu(DisplayCore* core);
void openTempMenu(DisplayCore* core);
void openPosMenu(DisplayCore* core);

#endif //MCEC_V5_CONSTANTS_H