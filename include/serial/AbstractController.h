#ifndef MCEC_V5_ABSTRACT_CONTROLLER_H
#define MCEC_V5_ABSTRACT_CONTROLLER_H

#include <stdint.h>
#include "../api.h"
#include "../serial_definitions.h"

#define CONTROLLER_VALUE_COUNT 16

struct InputState
{
    int8_t value;
    bool isNew;
    unsigned long modified;
};

class AbstractController
{
private:
    InputState state[CONTROLLER_VALUE_COUNT];
    int8_t prevControllerState[CONTROLLER_VALUE_COUNT];
    pros::Controller* controller = nullptr;

public:
    AbstractController();
    AbstractController(pros::Controller* controller);

    /**
     * Get the value for an analog input
     * 
     * @param analog The analog input to read from
     * @return int8_t The signed byte value (-127 to 127) returned
     */
    int8_t getAnalog(ControllerInput analog);

    /**
     * Get the boolean value for a digital input
     * 
     * @param digital The digital input to read from
     * @return true The button is currently pressed
     * @return false The button is not currently pressed
     */
    bool getDigital(ControllerInput digital);

    /**
     * Get if the digital input has registered a new press
     * Resets to "false" after being called
     * 
     * @param digital The digital input to read from
     * @return true The button has been newly pressed
     * @return false The button has not been newly pressed
     */
    bool getDigitalNewPress(ControllerInput digital);

    /**
     * Get if the digital input has registered a new press
     * DOES NOT reset to false after being called
     * 
     * @param digital The digital input to read from
     * @return true The button has been newly pressed
     * @return false The button has not been newly pressed
     */
    bool peekDigitalNewPress(ControllerInput digital);

    /**
     * Get if the digital input has been newly released from being pressed
     * Resets to "false" after being called
     * 
     * @param digital The digital input to read from
     * @return true The button has been newly released
     * @return false The button has not been newly released
     */
    bool getDigitalNewRelease(ControllerInput digital);

    /**
     * Get if the digital input has been newly released from being pressed
     * DOES NOT reset to "false" after being called
     * 
     * @param digital The digital input to read from
     * @return true The button has been newly released
     * @return false The button has not been newly released
     */
    bool peekDigitalNewRelease(ControllerInput digital);

    /**
     * Gets if the analog input has a new value
     * Resets to "false" after being called
     * 
     * @param analog The analog input to read from
     * @return true The analog input has a new value
     * @return false The analog input does not have a new value
     */
    bool isAnalogNew(ControllerInput analog);

    /**
     * Gets if the analog input has a new value
     * DOES NOT reset to "false" after being called
     * 
     * @param analog The analog input to read from
     * @return true The analog input has a new value
     * @return false The analog input does not have a new value
     */
    bool peekIsAnalogNew(ControllerInput analog);

    /**
     * Get the time in milliseconds that the input was last modified
     * 
     * @param input The input to check
     * @return unsigned long The time in milliseconds the input changed
     */
    unsigned long getLastTimeModified(ControllerInput input);

    /**
     * Get the time in milliseconds since the input was last modified
     * 
     * @param input The input to checl
     * @return unsigned long The time in milliseconds since the input changed
     */
    unsigned long timeSinceModified(ControllerInput input);

    /**
     * Detaches the controller to ignore inputs
     */
    void detachController();

    /**
     * Attaches a controller (replacing the existing one) to read inputs from
     * 
     * @param controller The controller to read inputs from
     */
    void attachController(pros::Controller* controller);

    /**
     * Checks the attached controller for new inputs
     * NOTE: Without calling this function, no new inputs will be registered
     */
    void checkController();

    /**
     * Manually set a controller value. Used for external controllers
     * 
     * @param input The input to modify
     * @param value The value to set the input to
     */
    void setValue(ControllerInput input, int8_t value);
};

#endif //MCEC_V5_ABSTRACT_CONTROLLER_H