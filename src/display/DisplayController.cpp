#include "displayLib/DisplayController.h"

const int DisplayController::charLengths[128] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 3, 8, 6, 10, 8, 1, 2, 2, 6, 7, 1, 3, 1, 5, 6, 3, 6, 6, 7, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 5, 10, 8, 6, 7, 7, 5, 5, 8, 7, 3, 3, 6, 5, 9, 7, 8, 6, 8, 6, 6, 7, 7, 8, 12, 7, 7, 6, 3, 5, 3, 6, 6, 2, 6, 6, 5, 6, 6, 4, 6, 6, 1, 3, 5, 1, 9, 6, 6, 6, 6, 4, 5, 4, 6, 6, 10, 5, 6, 5, 4, 1, 4, 6, 1, 1};

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

int DisplayController::getLength(std::string line)
{
    int length = 0;
    for(int i = 0; i < line.length(); i++)
    {
        char current = line[i];
        if(current > 128)
        {
            length += 2;
        }
        else
        {
            length += DisplayController::charLengths[current] + 1;
        }
    }
    length--;
    return length;
}