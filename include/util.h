#ifndef MCEC_V5_UTIL_H
#define MCEC_V5_UTIL_H

#include "devices.h"
#include "constants.h"

/**
 * Set the tray position to a particular point with specified speed
 *
 * @param trayPos The position to set the tray. With the current bot setup, this goes from 0 to negative as the tray goes out
 * @param speed The speed from 0-200 to move the tray
 */
void setTrayPosition(int trayPos, int speed);

/**
 * Used when moving the arms
 * Checks the arm position against a list of bounds to set the tray position accordingly
 */
void checkTrayArmsPos();

/**
 * Flip the tray out by moving the arms and tray violently
 */
void flipTray();

/**
 * Sets up the motor brake modes
 */
void setupMotors();

/**
 * Moves the tray to its highest, slowing it as it reaches the apex
 * Note: This is a blocking function
 */
void moveTrayToHighest();

#endif //MCEC_V5_UTIL_H