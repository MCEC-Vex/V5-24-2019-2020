#ifndef MCEC_V5_DEVICES_H
#define MCEC_V5_DEVICES_H

#include "api.h"
#include "okapi/api.hpp"
#include "displayLib/DisplayController.h"
using namespace okapi::literals;

// Motors are numbered from left to right
extern pros::Motor leftTopMotor;

extern pros::Motor leftBottomMotor;
extern pros::Motor rightTopMotor;
extern pros::Motor rightBottomMotor;
extern pros::Motor centerWheel;

extern pros::Motor leftIntake;
extern pros::Motor rightIntake;

extern pros::Motor leftArmMotor;
extern pros::Motor rightArmMotor;

extern pros::Motor trayMotorBack;
extern pros::Motor trayMotorFront;

extern pros::ADIPort autoRedSmall;
extern pros::ADIPort autoRedBig;
extern pros::ADIPort autoBlueSmall;
extern pros::ADIPort autoBlueBig;
extern pros::ADIPort tipSensor;
//pros::ADIPort autoSafe(5);
//pros::ADIPort autoTesting(6);
extern pros::ADIUltrasonic frontUltrasonic;
extern pros::ADIUltrasonic backUltrasonic;
extern pros::ADIUltrasonic rearUltrasonic;

extern okapi::MedianFilter<5> frontUltrasonicFilter;
extern okapi::MedianFilter<5> backUltrasonicFilter;
extern okapi::MedianFilter<5> rearUltrasonicFilter;


extern pros::Controller master;

extern DisplayController displayController;

extern std::shared_ptr<okapi::ChassisController> chassis;
extern std::shared_ptr<okapi::AsyncMotionProfileController> profileController;
#endif //MCEC_V5_DEVICES_H
