# Part1:  
## bash script (bullseye):
![StateChart](./images/p1_delay_bash_script.PNG)
CH2: input connected to pin19  
CH1: output connected to pin17  

There is a 1.77 ms delay   


## bash script using gpiod

    give permissions to file using: chmod u+x flash_new.sh
    Execute bash script using: ./flash_new.sh & 
    The "&" sends the process to the background and the cpu load can be determined using the "top" command:

    PID USER PR  NI  VIRT   RES   SHR S  %CPU  %MEM  TIME+ CMD 
    2493 pi  20   0  8128  3500  2132 R  22.8   0.8  0:09.64 bash

    pi@rpi-matthiasr24:~/Linux_Motor_Encoder/part1 $ kill 2493
    [1]+  Terminated              ./flash_new.sh

