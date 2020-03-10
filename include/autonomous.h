#ifndef MCEC_V5_AUTONOMOUS_H
#define MCEC_V5_AUTONOMOUS_H

#include "api.h"
#include "okapi/api.hpp"
#include "devices.h"
#include "util.h"
#include <cmath>
using namespace okapi::literals;

/**
 * Run auton spicy mode (new and fresh with a lil bit of kick)
 * 
 * @param red True if the bot is on the red alliance, false if on blue
 */
void runSpicyAuton(bool red);

/**
 * Run auton mild mode (fewer turns)
 * 
 * @param red True if the bot is on the red alliance, false if on blue
 */
void runMildAuton(bool red);

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

#endif //MCEC_V5_AUTONOMOUS_H
