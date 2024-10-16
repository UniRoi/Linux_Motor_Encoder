#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

int main(){
    char in_buffer[65535];
    char out_buffer[1] = {'\0'};
    int count = 1;
    volatile uint16_t ui16EncoderPos = 0;

    while(true){
        sleep(1);

        int fd = open("/dev/encoder", O_RDWR);
        if(fd < 0) {
            printf("Failed to open encoder.\n");
            return -1;
        }   
        ui16EncoderPos = read(fd, in_buffer, count);
        printf("Data read: %hd\n", ui16EncoderPos);   

        /* Example how to write the value --> can used to reset position */
        // if(ui16EncoderPos == 20)
        // {
        //     int ret = write(fd, out_buffer, count);
        //     printf("Data to write %d\n", *out_buffer);   

        //     if(ret < 0) {
        //         printf("Failed to write.\n");
        //         return -1;
        //     } else if(ret == 0) {
        //         printf("No data was written.\n");
        //     } else {
        //         printf("Data written\n");
        //     }
        // }  

        close(fd);
    }

    return 0;
}


