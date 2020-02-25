#include "serial/AbstractController.h"

AbstractController::AbstractController() : AbstractController(nullptr)
{

}

AbstractController::AbstractController(pros::Controller* controller)
{
    this->attachController(controller);

    // Reset controller values to initial
    for(int i = 0; i < CONTROLLER_VALUE_COUNT; i++)
    {
        this->state[i].isNew = false;
        this->state[i].modified = 0;
        this->state[i].value = 0;

        this->prevControllerState[i] = 0;
    }
}

int8_t AbstractController::getAnalog(ControllerInput analog)
{
    return this->state[analog].value;
}

bool AbstractController::getDigital(ControllerInput digital)
{
    return this->state[digital].value;
}

bool AbstractController::getDigitalNewPress(ControllerInput digital)
{
    if(this->state[digital].value == true && this->state[digital].isNew)
    {
        this->state[digital].isNew = false;
        return true;
    }
    return false;
}

bool AbstractController::peekDigitalNewPress(ControllerInput digital)
{
    return (this->state[digital].value == true && this->state[digital].isNew);
}

bool AbstractController::getDigitalNewRelease(ControllerInput digital)
{
    if(this->state[digital].value == false && this->state[digital].isNew)
    {
        this->state[digital].isNew = false;
        return true;
    }
    return false;
}

bool AbstractController::peekDigitalNewRelease(ControllerInput digital)
{
    return (this->state[digital].value == false && this->state[digital].isNew);
}

bool AbstractController::isAnalogNew(ControllerInput analog)
{
    if(this->state[analog].isNew)
    {
        this->state[analog].isNew = false;
        return true;
    }
    return false;
}

bool AbstractController::peekIsAnalogNew(ControllerInput analog)
{
    return this->state[analog].isNew;
}

unsigned long AbstractController::getLastTimeModified(ControllerInput input)
{
    return this->state[input].modified;
}

unsigned long AbstractController::timeSinceModified(ControllerInput input)
{
    return pros::millis() - this->state[input].modified;
}

void AbstractController::detachController()
{
    this->controller = nullptr;
}

void AbstractController::attachController(pros::Controller* controller)
{
    this->controller = controller;
}

void AbstractController::checkController()
{
    if(this->controller == nullptr)
    {
        return;
    }

    // Analog values
    for(int i = 0; i < 4; i++)
    {
        int8_t value = this->controller->get_analog((pros::controller_analog_e_t) i);
        if(value != this->prevControllerState[i])
        {
            //TODO callback with previous state and new state
            this->prevControllerState[i] = value;
            this->state[i].isNew = true;
            this->state[i].modified = pros::millis();
            this->state[i].value = value;
        }
    }

    // Digital values. Offset at 2??
    for(int i = (4 + 2); i < (16 + 2); i++)
    {
        int8_t value = this->controller->get_digital((pros::controller_digital_e_t) i);
        if(value != this->prevControllerState[i - 2])
        {
            //TODO callback with previous state and new state
            this->prevControllerState[i] = value;
            this->state[i].isNew = true;
            this->state[i].modified = pros::millis();
            this->state[i].value = value;
        }
    }
}

void AbstractController::setValue(ControllerInput input, int8_t value)
{
    if(this->state[input].value != value)
    {
        this->state[input].value = value;
        this->state[input].modified = pros::millis();
        this->state[input].isNew = true;
    }
}