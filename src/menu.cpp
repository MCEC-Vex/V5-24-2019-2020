#include "menu.h"

void openMainMenu(DisplayController* controller, pros::Controller master)
{
    DisplayCore core(controller, master);

    MenuAction extendTray([](DisplayCore* core)
    {
        core->popScreen();
        moveTrayToHighest();
    }, "Extend tray", &core);

    MenuAction flipTrayAction([](DisplayCore* core)
    {
        core->popScreen();
        flipTray();
    }, "Flip tray", &core);

    MenuAction forwardAsymmetryTests([](DisplayCore* core)
    {
        runAsymmetryForwardTests(10);
        core->popScreen();
    }, "Forward asym", &core);

    MenuAction turnAsymmetryTests([](DisplayCore* core)
    {
        runAsymmetryTurningTests(10);
        core->popScreen();
    }, "Turn asym", &core);

    MenuAction temps([](DisplayCore* core)
    {
        openTempMenu(core);
    }, "Motor Temps", &core);

    MenuAction positions([](DisplayCore* core)
    {
        openPosMenu(core);
    }, "Positions", &core);

    MenuAction auton([](DisplayCore* core)
    {
        openAutoMenu(core);
    }, "Auton...", &core);

    MenuAction tank([](DisplayCore* core)
    {
        tankMode = !tankMode;
        core->popScreen();
    }, tankMode ? "Turn tank off" : "Turn tank on", &core);

    MenuAction menu[8] = {
        auton,
        extendTray,
        flipTrayAction,
        turnAsymmetryTests,
        forwardAsymmetryTests,
        temps,
        positions,
        tank
    };
    MenuScreen screen(&core, menu, 8);

    core.pushScreen(&screen);
    core.waitForEmpty();
}

void openAutoMenu(DisplayCore* core)
{
    MenuAction spicyRed([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runSpicyAuton(true);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Spicy red", core);

    MenuAction spicyBlue([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runSpicyAuton(false);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Spicy blue", core);

    MenuAction mildRed([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runMildAuton(true);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Mild red", core);

    MenuAction mildBlue([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runMildAuton(false);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Mild blue", core);

    MenuAction menu[4] = {
        mildRed,
        mildBlue,
        spicyRed,
        spicyBlue
    };
    MenuScreen screen(core, menu, 4);

    core->pushScreen(&screen);
    core->waitForPop();
}

void openTempMenu(DisplayCore* core)
{
    std::string tempDisplay[17];
    tempDisplay[0] = "Intake:";
    tempDisplay[1] = "L: " + std::to_string(leftIntake.get_temperature());
    tempDisplay[2] = "R: " + std::to_string(rightIntake.get_temperature());
    tempDisplay[3] = "Drive:";
    tempDisplay[4] = "LT: " + std::to_string(leftTopMotor.get_temperature());
    tempDisplay[5] = "LB: " + std::to_string(leftBottomMotor.get_temperature());
    tempDisplay[6] = "RT: " + std::to_string(rightTopMotor.get_temperature());
    tempDisplay[7] = "RB: " + std::to_string(rightBottomMotor.get_temperature());
    tempDisplay[8] = "Arms:";
    tempDisplay[9] = "L: " + std::to_string(leftArmMotor.get_temperature());
    tempDisplay[10] = "R: " + std::to_string(rightArmMotor.get_temperature());
    tempDisplay[11] = "Tray:";
    tempDisplay[12] = "F: " + std::to_string(trayMotorFront.get_temperature());
    tempDisplay[13] = "B: " + std::to_string(trayMotorBack.get_temperature());
    tempDisplay[11] = "Anti-Tip:";
    tempDisplay[12] = "L: " + std::to_string(leftAntiTip.get_temperature());
    tempDisplay[13] = "R: " + std::to_string(rightAntiTip.get_temperature());

    TextDisplayScreen temps(core, tempDisplay, 17);
    core->pushScreen(&temps);
    core->waitForPop();
    core->popScreen();
}

void openPosMenu(DisplayCore* core)
{
    std::string posDisplay[9];
    posDisplay[0] = "Arms:";
    posDisplay[1] = "L: " + std::to_string(leftArmMotor.get_position());
    posDisplay[2] = "R: " + std::to_string(rightArmMotor.get_position());
    posDisplay[3] = "Tray:";
    posDisplay[4] = "F: " + std::to_string(trayMotorFront.get_position());
    posDisplay[5] = "B: " + std::to_string(trayMotorBack.get_position());
    posDisplay[6] = "Anti-Tip:";
    posDisplay[7] = "L: " + std::to_string(leftAntiTip.get_position());
    posDisplay[8] = "R: " + std::to_string(rightAntiTip.get_position());

    TextDisplayScreen positions(core, posDisplay, 9);
    core->pushScreen(&positions);
    core->waitForPop();
    core->popScreen();
}