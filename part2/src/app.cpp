#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

int main(){
    char out_buffer[100];
    int count = 0;
    int offset = 0;
    volatile uint16_t ui16EncoderPos = 0;

    while(true){
        sleep(1);

        int fd = open("/dev/encoder", O_RDWR);
        if(fd < 0) {
            printf("Failed to open encoder.\n");
            return -1;
        }   

        ui16EncoderPos = read(fd, out_buffer, count);
        printf("Data read: %hd\n", ui16EncoderPos);     

        close(fd);
    }

    return 0;
}


