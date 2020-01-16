#include "../include/DisplayController.h"

DisplayController::DisplayController(pros::Controller controller1) : controller(controller1)
{
    //pros::Task testingTask(this->runTask, (void*)"", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT);
}

void DisplayController::sendNext()
{
    //double rand = random();
    //controller.set_text(0, 0, std::to_string(rand).c_str());

    mutex.take(1000);

    bool didUpdate = false;

    int current = lastUpdatedLine;
    for(int i = 0; i < 4; i++)
    {
        current++;
        if(current > 2)
        {
            current = -1;
        }

        if(current == -1 && !rumbleLine.empty())
        {
            // Send rumble
            controller.rumble(rumbleLine.c_str());
            rumbleLine = "";
            lastUpdatedLine = -1;
            break;
        }
        else
        {
            if(modified[current])
            {
                controller.set_text(current, 0, lines[current].c_str());
                modified[current] = false;
                lastUpdatedLine = current;
                break;
            }
        }
    }

    /*if(!rumbleLine.empty() && lastUpdatedLine != -1)
    {
        controller.rumble(rumbleLine.c_str());
        rumbleLine = "";
        lastUpdatedLine = -1;
        didUpdate = true;
    }
    else
    {
        // Go through the lines
        for(int i = 0; i < 3; i++)
        {
            if(modified[i] && lastUpdatedLine != i)
            {
                controller.set_text(i, 0, lines[i].c_str());

                modified[i] = false;
                lastUpdatedLine = i;
                didUpdate = true;
                break;
            }
        }
    }

    if(!didUpdate)
    {
        lastUpdatedLine = -2;
    }*/

    mutex.give();
}

void DisplayController::setLine(int line, std::string data)
{
    if(line < 0 || line > 2)
    {
        //TODO throw a proper error
        return;
    }

    // Take the mutex to ensure thread safety
    mutex.take(1000);

    //data.insert(0, 10 - data.length(), ' ');

    lines[line] = data;
    modified[line] = true;

    mutex.give();
}

void DisplayController::clearLine(int line)
{
    // 10 spaces for character limit
    setLine(line, "          ");
}

void DisplayController::rumble(std::string pattern)
{
    mutex.take(1000);
    rumbleLine = pattern;
    mutex.give();
}