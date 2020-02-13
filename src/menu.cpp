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

    MenuAction alignBot([](DisplayCore* core)
    {
        core->popScreen();
        //centerBot(5, 0.2);
    }, "Align bot", &core);

    MenuAction moveAlongWall([](DisplayCore* core)
    {
        core->popScreen();
    }, "Test wall 10x", &core);

    MenuAction runAuton([](DisplayCore* core)
    {
        core->popScreen();
        //autonomous();
    }, "Auton Sim", &core);

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
    //    autonBig,
    //    autonSmall,
        auton,
        extendTray,
        flipTrayAction,
        alignBot,
        temps,
        moveAlongWall,
        positions,
        tank
    //    runAuton
    };
    MenuScreen screen(&core, menu, 8);

    core.pushScreen(&screen);
    //ScrollingScreenDemo demo(&core);
    //core.pushScreen(&demo);
    core.waitForEmpty();
}

void openAutoMenu(DisplayCore* core)
{
    MenuAction bigRed([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runAutoBig(true);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Big red", core);

    MenuAction smallRed([](DisplayCore* core)
    {
        core->popScreen();
        runAutoSmall(true);
    }, "Small red", core);

    MenuAction bigBlue([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runAutoBig(false);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Big blue", core);

    MenuAction smallBlue([](DisplayCore* core)
    {
        core->popScreen();
        runAutoSmall(false);
    }, "Small blue", core);

    MenuAction redFast([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runAutoBigFast(true);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Big red fast", core);

    MenuAction blueFast([](DisplayCore* core)
    {
        core->popScreen();

        unsigned long startTime = pros::millis();
        runAutoBigFast(false);
        unsigned long timeTaken = pros::millis() - startTime;

        std::string tempDisplay[2];
        tempDisplay[0] = "Time taken:";
        tempDisplay[1] = std::to_string(timeTaken);
        TextDisplayScreen timeTakenText(core, tempDisplay, 2);
        core->pushScreen(&timeTakenText);
        core->waitForPop();
    }, "Big blue fast", core);

    MenuAction menu[6] = {
        bigRed,
        smallRed,
        bigBlue,
        smallBlue,
        redFast,
        blueFast
    };
    MenuScreen screen(core, menu, 6);

    core->pushScreen(&screen);
    core->waitForPop();
}

void openTempMenu(DisplayCore* core)
{
    std::string tempDisplay[14];
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

    TextDisplayScreen temps(core, tempDisplay, 14);
    core->pushScreen(&temps);
    core->waitForPop();
    core->popScreen();
}

void openPosMenu(DisplayCore* core)
{
    std::string posDisplay[6];
    posDisplay[0] = "Arms:";
    posDisplay[1] = "L: " + std::to_string(leftArmMotor.get_position());
    posDisplay[2] = "R: " + std::to_string(rightArmMotor.get_position());
    posDisplay[3] = "Tray:";
    posDisplay[4] = "F: " + std::to_string(trayMotorFront.get_position());
    posDisplay[5] = "B: " + std::to_string(trayMotorBack.get_position());

    TextDisplayScreen positions(core, posDisplay, 6);
    core->pushScreen(&positions);
    core->waitForPop();
    core->popScreen();
}

void openSensorMenu(DisplayCore* core)
{

}