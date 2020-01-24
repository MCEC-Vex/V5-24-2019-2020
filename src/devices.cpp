#include "devices.h"

//bad ports: 5,6,21,12,15

pros::Motor leftTopMotor(1);
pros::Motor leftBottomMotor(2);
pros::Motor rightTopMotor(3);
pros::Motor rightBottomMotor(4);
pros::Motor centerWheel(15);

pros::Motor leftIntake(11);
pros::Motor rightIntake(16);

pros::Motor leftArmMotor(8);
pros::Motor rightArmMotor(9);

pros::Motor trayMotorBack(13);
pros::Motor trayMotorFront(14);

pros::ADIPort autoRedSmall(1);
pros::ADIPort autoRedBig(2);
pros::ADIPort autoBlueSmall(3);
pros::ADIPort autoBlueBig(4);
pros::ADIPort tipSensor(1);
//pros::ADIPort autoSafe(5);
//pros::ADIPort autoTesting(6);
pros::ADIUltrasonic frontUltrasonic(7, 8);
pros::ADIUltrasonic backUltrasonic(5, 6);
pros::ADIUltrasonic rearUltrasonic(3, 4);
okapi::MedianFilter<5> frontUltrasonicFilter;
okapi::MedianFilter<5> backUltrasonicFilter;
okapi::MedianFilter<5> rearUltrasonicFilter;


pros::Controller master(pros::E_CONTROLLER_MASTER);

DisplayController displayController(master);

std::shared_ptr<okapi::ChassisController> chassis = okapi::ChassisControllerBuilder()
        // Left side is 1,2 right side is -3,-4 (negative indicates reversed)
        .withMotors({1, 2}, {-3, -4})
        .withDimensions(okapi::AbstractMotor::gearset::green, {{4_in, 12.5_in}, okapi::imev5GreenTPR})
        .build();

std::shared_ptr<okapi::AsyncMotionProfileController> profileController = okapi::AsyncMotionProfileControllerBuilder()
    .withLimits({1.0, 2.0, 10.0})
    .withOutput(chassis)
    .buildMotionProfileController();
