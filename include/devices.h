#ifndef MCEC_V5_DEVICES_H
#define MCEC_V5_DEVICES_H

#include "api.h"
#include "okapi/api.hpp"
#include "displayLib/DisplayController.h"

// Motors are numbered from left to right
pros::Motor leftTopMotor(1);
pros::Motor leftBottomMotor(2);
pros::Motor rightTopMotor(3);
pros::Motor rightBottomMotor(4);
pros::Motor centerWheel(15);

pros::Motor leftIntake(11);
pros::Motor rightIntake(12);

pros::Motor leftArmMotor(8);
pros::Motor rightArmMotor(9);

pros::Motor trayMotorBack(13);
pros::Motor trayMotorFront(14);

pros::ADIPort autoRedSmall(1);
pros::ADIPort autoRedBig(2);
pros::ADIPort autoBlueSmall(3);
pros::ADIPort autoBlueBig(4);
//pros::ADIPort autoSafe(5);
//pros::ADIPort autoTesting(6);
pros::ADIUltrasonic frontUltrasonic(7, 8);
pros::ADIUltrasonic backUltrasonic(5, 6);
okapi::MedianFilter<5> frontUltrasonicFilter;
okapi::MedianFilter<5> backUltrasonicFilter;

pros::Controller master(pros::E_CONTROLLER_MASTER);

DisplayController displayController(master);

auto chassis = okapi::ChassisControllerBuilder()
        // Left side is 1,2 right side is -3,-4 (negative indicates reversed)
        .withMotors({1, 2}, {-3, -4})
        .withDimensions(okapi::AbstractMotor::gearset::green, {{4_in, 12.5_in}, okapi::imev5GreenTPR})
        .build();

#endif //MCEC_V5_DEVICES_H