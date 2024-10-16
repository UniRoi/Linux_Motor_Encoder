#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(){
    char out_buffer[100];
    int count = 0;
    int offset = 0;

    while(true){
        sleep(1);

        int fd = open("/dev/encoder", O_RDWR);
        if(fd < 0) {
            printf("Failed to open encoder.\n");
            return -1;
        }   

        int ret = read(fd, out_buffer, count);     
        if(ret < 0) {
            printf("Failed to read.\n");
            return -1;
        } else if(ret == 0) {
            printf("No data was read.\n");
        } else {

            printf("Data read: %d\n", ret);
        }

        close(fd);
    }

    return 0;
}


