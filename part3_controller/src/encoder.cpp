#include <encoder.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>


encoder::encoder()
{

}

encoder::~encoder()
{
    // close(fd);
}

int encoder::init(void)
{
    int ret = 0;
    /* check if kernel module is loaded*/
    fd = open("/dev/encoder", O_RDWR);
    if(fd > 0)
    {
        /* do a reset */
        ResetPosition();
    }
    else
    {
        std::cout << "Could not open encoder device\n";
        ret = -1;
    }
    close(fd);
    
    return ret;
}



/* just */
uint16_t encoder::GetPosition(void)
{
    int count = 1;
    char in_buffer[1] = {0};
    uint16_t u16EncoderCnt = 0;
    fd = open("/dev/encoder", O_RDWR);
    u16EncoderCnt = read(fd, in_buffer, count);
    close(fd);
    return u16EncoderCnt;
}

void encoder::ResetPosition(void)
{
    char out_buffer[1] = {0};
    int ret = 0;
    int count = 1;
    /* must be done with a write to the kernel module */
    
    /* only call of reset */
    // ret = write(fd, out_buffer, strlen(out_buffer));
    fd = open("/dev/encoder", O_RDWR);
    ret = write(fd, out_buffer, count);
    if (ret < 0)
    {
        std::cout << "could not reset LKM\n";
    }
    close(fd);
}


void encoder::updatePps(void)
{
        /* Calculate Rps with timebase of expired timer */
        i16MotorPps = GetPosition();
        ResetPosition();
}


int16_t encoder::GetPps(void)
{
    return i16MotorPps * 50;
}

int16_t encoder::GetRpm(void)
{
    return static_cast<int16_t>((GetPps() / 7.0) * 60);
}
