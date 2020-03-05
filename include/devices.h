#ifndef MCEC_V5_DEVICES_H
#define MCEC_V5_DEVICES_H

#include "api.h"
#include "okapi/api.hpp"
#include "displayLib/DisplayController.h"
#include "serial/AbstractController.h"

using namespace okapi::literals;

extern pros::Motor leftTopMotor;
extern pros::Motor leftBottomMotor;
extern pros::Motor rightTopMotor;
extern pros::Motor rightBottomMotor;

extern pros::Motor leftIntake;
extern pros::Motor rightIntake;

extern pros::Motor leftArmMotor;
extern pros::Motor rightArmMotor;

extern pros::Motor trayMotorBack;
extern pros::Motor trayMotorFront;

extern pros::Motor leftAntiTip;
extern pros::Motor rightAntiTip;

extern pros::ADIPort autoRedSmall;
extern pros::ADIPort autoRedBig;
extern pros::ADIPort autoBlueSmall;
extern pros::ADIPort autoBlueBig;
extern pros::ADIPort disableMenu;

extern pros::Controller master;
extern AbstractController abstractController;

extern DisplayController displayController;

extern std::shared_ptr<okapi::ChassisController> chassis;
extern std::shared_ptr<okapi::AsyncMotionProfileController> profileController;
extern std::shared_ptr<okapi::AsyncMotionProfileController> profileControllerFast;

extern bool tankMode;
extern bool antiTipEnabled;

#endif //MCEC_V5_DEVICES_H
