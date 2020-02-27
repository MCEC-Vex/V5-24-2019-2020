#ifndef MCEC_V5_CONSTANTS_H
#define MCEC_V5_CONSTANTS_H

// Motor positions
//TODO document the process for obtaining these values
#define TRAY_LOWEST 0
#define TRAY_HIGHEST 3250

// Arm software limits for bot safety
#define ARMS_LOWEST 0
#define ARMS_HIGHEST 1770 // Sea shanties
#define ARMS_SPEED 100

#define TRAY_SHIFT_SPEED 200

#define TIP_LOW_POS 1400

#define ANTI_TIP_DECEL_TIME 5000.0
#define ANTI_TIP_SCALE 3.5

// Control definitions
#define FORWARD_POWER STICK_LEFT_Y
#define TURNING_POWER STICK_RIGHT_X

#define ARMS_UP BUTTON_UP
#define ARMS_DOWN BUTTON_DOWN

#define BACKWARDS_OUT BUTTON_B
#define ROLLER_OUTTAKE BUTTON_R2
#define ROLLER_INTAKE BUTTON_R1

#define TRAY_OUT BUTTON_L1
#define TRAY_IN BUTTON_L2

#define ARM_MACRO BUTTON_X

#endif //MCEC_V5_CONSTANTS_H
