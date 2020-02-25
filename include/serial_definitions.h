#ifndef SERIAL_DEFINITIONS_H
#define SERIAL_DEFINITIONS_H

#include <stdint.h>

enum StreamFlag : uint8_t
{
    FORWARD,
    PARSE_RAW,
    PARSE_MSGPACK
};

enum PacketType : uint8_t
{
    CLEAR,
    PRINT_DEMO,
    ENABLE_WIFI,
    PACK_PING,
    PONG,
    ANTI_TIP,
    CONTROL,
    ULTRASONIC_SETPOS
};

enum UltrasonicGroup : uint8_t
{
    ULTRASONIC_LEFT,
    ULTRASONIC_RIGHT,
    ULTRASONIC_BACK
};

enum ControllerInput : uint8_t
{
    STICK_LEFT_X,
    STICK_LEFT_Y,
    STICK_RIGHT_X,
    STICK_RIGHT_Y,

    BUTTON_L1,
    BUTTON_L2,
    BUTTON_R1,
    BUTTON_R2,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_X,
    BUTTON_B,
    BUTTON_Y,
    BUTTON_A
};

struct PacketHeader
{
    StreamFlag flag;
    PacketType type;
};

struct ClearPacket
{
    uint8_t line;
};

struct AntiTipPacket
{
    float gyroY;
};

struct PrintDemoPacket
{
    uint8_t line;
};

struct UltrasonicPosPacket
{
    UltrasonicGroup group;
    uint8_t position;
};

struct ControlPacket
{
    ControllerInput input;
    int8_t value;
};

#endif //SERIAL_DEFINITIONS_H