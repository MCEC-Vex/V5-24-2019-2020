#include "devices.h"

// Bad ports: 5,6,21,12,11,19
pros::Motor leftTopMotor(8);
pros::Motor leftBottomMotor(9);
pros::Motor rightTopMotor(7);
pros::Motor rightBottomMotor(10);

pros::Motor leftIntake(17);
pros::Motor rightIntake(16);

pros::Motor leftArmMotor(14);
pros::Motor rightArmMotor(13);

pros::Motor trayMotorBack(3);
pros::Motor trayMotorFront(4);

pros::Motor leftAntiTip(18);
pros::Motor rightAntiTip(15);

pros::ADIPort autoRedSmall(1);
pros::ADIPort autoRedBig(2);
pros::ADIPort autoBlueSmall(3);
pros::ADIPort autoBlueBig(4);
pros::ADIPort disableBrake(7);
pros::ADIPort disableMenu(8);

pros::Controller master(pros::E_CONTROLLER_MASTER);

AbstractController abstractController(&master);
DisplayController displayController(master);

std::shared_ptr<okapi::ChassisController> chassis = okapi::ChassisControllerBuilder()
        // Left side is 1,2 right side is -3,-4 (negative indicates reversed)
        .withMotors({8, 9}, {7, 10})
        .withDimensions(okapi::AbstractMotor::gearset::green, {{4_in, 9.5_in}, okapi::imev5GreenTPR})
        .build();

std::shared_ptr<okapi::AsyncMotionProfileController> profileController = okapi::AsyncMotionProfileControllerBuilder()
    .withLimits({0.5, 1.0, 5.0})
    .withOutput(chassis)
    .buildMotionProfileController();

std::shared_ptr<okapi::AsyncMotionProfileController> profileControllerFast = okapi::AsyncMotionProfileControllerBuilder()
    .withLimits({0.8, 1.5, 7.5})
    .withOutput(chassis)
    .buildMotionProfileController();

std::shared_ptr<okapi::AsyncMotionProfileController> profileControllerChonky = okapi::AsyncMotionProfileControllerBuilder()
    .withLimits({0.2, 0.4, 2.0})
    .withOutput(chassis)
    .buildMotionProfileController();

bool tankMode = false;
bool antiTipEnabled = false;
bool armsBrake = false;