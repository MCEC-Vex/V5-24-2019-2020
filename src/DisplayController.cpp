#include "../include/DisplayController.h"

DisplayController::DisplayController(pros::Controller controller1) : controller(controller1)
{
    
}

void DisplayController::sendNext()
{
    mutex.take(1000);

    // Do a cycle that ends at the item most recently updated
    // This gives all the lines (and rumble) a chance to be updated in a timely manner regardles of write frequency
    bool didUpdate = false;
    int current = lastUpdatedLine;
    for(int i = 0; i < 4; i++)
    {
        current++;
        if(current > 2)
        {
            current = -1;
        }

        // Check if the rumble line is set
        if(current == -1 && !rumbleLine.empty())
        {
            // Send rumble (line -1)
            controller.rumble(rumbleLine.c_str());
            rumbleLine = "";
            lastUpdatedLine = -1;
            break;
        }
        else
        {
            if(modified[current])
            {
                // Send the new line and switch the modified flag off
                controller.set_text(current, 0, lines[current].c_str());
                modified[current] = false;
                lastUpdatedLine = current;
                break;
            }
        }
    }
    mutex.give();
}

void DisplayController::setLine(int line, std::string data)
{
    if(line < 0 || line > 2)
    {
        //TODO throw a proper error
        return;
    }

    if(data.size() > 16)
    {
        data.erase(16, std::string::npos);
    }

    // Take the mutex to ensure thread safety
    mutex.take(1000);

    lines[line] = data.insert(data.length(), 16 - data.length(), ' ');
    lines[line] = data;
    modified[line] = true;

    mutex.give();
}

void DisplayController::clearLine(int line)
{
    // 16 spaces for character limit
    setLine(line, "                ");
}

void DisplayController::rumble(std::string pattern)
{
    mutex.take(1000);
    rumbleLine = pattern;
    mutex.give();
}