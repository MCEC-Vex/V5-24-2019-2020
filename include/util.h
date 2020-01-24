#ifndef MCEC_V5_UTIL_H
#define MCEC_V5_UTIL_H

#include "devices.h"
#include "constants.h"
#include "okapi/api.hpp"

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

/**
 * Get the difference between ultrasonic sensors on the right side
 * 
 * @return int The difference value. Positive indicates the bot is rotated clockwise
 */
int getUltrasonicDifference();

/**
 * Moves the chassis a distance while keeping the right side of the bot
 * parallel to a wall using okapilib methods
 * 
 * @param chassis
 * @param distance 
 */
void moveAlignedWithWallOkapi(std::shared_ptr<okapi::ChassisController> chassis, okapi::QLength distance);

/**
 * Moves the chassis a distance while keeping the right side of the bot
 * parallel to a wall
 * 
 * @param distance The distance to move
 */
void moveAlignedWithWall(int distance);

#endif //MCEC_V5_UTIL_H
