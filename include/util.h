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
 * Used when moving the arms
 * Checks the arm position against a list of bounds to set the tray position accordingly
 */
void checkTrayArmsPosOld();

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

double convertToEncoderUnits(std::shared_ptr<okapi::ChassisController> chassis, okapi::QLength distance);

/**
 * Blocks thread execution until the specificed motor is within tolerance
 * 
 * @param motor 
 * @param value 
 * @param tolerance 
 */
void waitUntilMotorWithin(pros::Motor motor, int value, int tolerance, int timeout);

void moveMotors(std::shared_ptr<okapi::ChassisController> chassis, okapi::QLength distance, int speed, int timeout);

void moveIntakeAtSharedSpeed(int velocity);

void setChassisBrakeMode(pros::motor_brake_mode_e_t brakeMode);

void tareChassis();

void moveChassisDistance(std::shared_ptr<okapi::ChassisController> chassis,
    okapi::QLength leftDistance, okapi::QLength rightDistance, int velocity);

void deployAntiTip();

void retractAntiTip();

#endif //MCEC_V5_UTIL_H
