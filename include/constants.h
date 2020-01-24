#ifndef MCEC_V5_CONSTANTS_H
#define MCEC_V5_CONSTANTS_H

// Motor positions
//TODO document the process for obtaining these values
#define LEFT_ARM_LOW_SAFE 216
#define RIGHT_ARM_LOW_SAFE -211
#define TRAY_LOWEST 0
#define TRAY_HIGHEST -1900
#define TRAY_ARM_SAFE -750
#define ARM_TOWER_LOW_LEFT 1500
#define ARM_TOWER_HIGH_LEFT 1910

// Arm software limits for bot safety
#define ARMS_LOWEST 0
#define ARMS_HIGHEST 1770 // Sea shanties

#define TRAY_SHIFT_SPEED 200

// Control definitions
#define ARMS_UP DIGITAL_UP
#define ARMS_DOWN DIGITAL_DOWN

#define BACKWARDS_OUT DIGITAL_B
#define ROLLER_OUTTAKE DIGITAL_R2
#define ROLLER_INTAKE DIGITAL_R1

#define TRAY_OUT DIGITAL_L1
#define TRAY_IN DIGITAL_L2

#define ARM_MACRO DIGITAL_X

#endif //MCEC_V5_CONSTANTS_H
