#ifndef MCEC_V5_AUTONOMOUS_H
#define MCEC_V5_AUTONOMOUS_H

#include "api.h"
#include "okapi/api.hpp"
#include "devices.h"
#include "util.h"
#include <cmath>
using namespace okapi::literals;

#define AUTON_BIG_INTAKE_SPEED 100
#define AUTON_BIG_INTAKE_SPEED_FAST 200

/**
 * Center the bot against the right wall
 */
void centerBot(int tolerance, float speedMod);

/**
 * Autonomous to gather the four cubes on the far right and stack them in the right corner
 *
 * @param red True if the bot is on the red alliance, false if on blue
 */
void runAutoSmall(bool red);

/**
 * Run auto for the sideways "L" of cubes near the right (quickly)
 * 
 * @param red True if the bot is on the red alliance, false if on blue
 */
void runAutoBigFast(bool red);

/**
 * Run auton spicy mode (new and fresh with a lil bit of kick)
 * 
 * @param red True if the bot is on the red alliance, false if on blue
 */
void runSpicyAuton(bool red);

/**
 * Run asymmetry tests for forwards + backwards
 * 
 * @param iterations The number of times to go back and forth
 */
void runAsymmetryForwardTests(int iterations);

/**
 * Run asymmetry tests for turning left / right
 * 
 * @param iterations The number of times to turn back and forth
 */
void runAsymmetryTurningTests(int iterations);

void moveDistanceParallel(okapi::QLength distance, okapi::QLength distanceFromWall);

void backupTurn(double forwardSpeed, double yaw, int leftLength);

#endif //MCEC_V5_AUTONOMOUS_H
