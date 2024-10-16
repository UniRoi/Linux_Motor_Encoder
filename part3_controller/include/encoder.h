#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stdbool.h>


class encoder
{
private:
    volatile int16_t i16EncoderPos = 0;
    int16_t i16MotorPps = 0;
    int fd = -1;
public:

    int init(void);

    uint16_t GetPosition(void);
    void ResetPosition(void);

    void updatePps(void);
    int16_t GetPps(void);
    int16_t GetRpm(void);


    encoder();
    ~encoder();
};

#endif