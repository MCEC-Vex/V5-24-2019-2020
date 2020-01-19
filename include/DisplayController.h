
#ifndef MCEC_V5_DISPLAYCONTROLLER_H
#define MCEC_V5_DISPLAYCONTROLLER_H

#include "api.h"

class DisplayController
{
private:
    // The controller to display lines on
    pros::Controller controller;
    // Mutex to ensure thread safety with modifying lines
    pros::Mutex mutex;
    // The most recently updated line. Starts at -2 because rumble is considered -1
    int lastUpdatedLine = -2;
    // The 3 string lines and boolean flags to track modification
    std::string lines[3];
    bool modified[3];
    // The rumble line. Emptiness indicates no modification
    std::string rumbleLine;

public:
    /**
     * Construct a new Display Controller object
     * 
     * @param controller The controller to use
     */
    DisplayController(pros::Controller controller);

    /**
     * Clears a line on the controller by writing 16 spaces
     * 
     * @param line The line number to clear (1-2)
     */
    void clearLine(int line);

    /**
     * Set the Line text and add it to the update cycle
     * 
     * @param line The line number to set (1-2)
     * @param data The string to write. Can be up to 16 characters
     */
    void setLine(int line, std::string data);

    /**
     * Adds rumbling to the update cycle
     * 
     * @param pattern The rumble pattern of up to 8 characters where . = short, - = long, and " " = pause
     */
    void rumble(std::string pattern);

    /**
     * Runs the next iteration of the update cycle. This should only be run every 50 milliseconds
     */
    void sendNext();
};


#endif //MCEC_V5_DISPLAYCONTROLLER_H
