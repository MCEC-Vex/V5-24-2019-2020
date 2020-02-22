#ifndef MCEC_V5_SERIAL_H
#define MCEC_V5_SERIAL_H

class Serial
{
public:
    typedef void (*packetHandlerFunction)(const uint8_t* buffer, size_t size);
};

#endif