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

![StateChart](./images/p1_max_bash_script.PNG)
CH2: input connected to pin20   
CH1: output connected to pin17  


If the CPU us stressed the delay gets bigger:

![StateChart](./images/p1_delay_bash_stress.PNG)
CH2: input connected to pin20   
CH1: output connected to pin17 



## programm

    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND                                              
    61128 rpi-sar+  20   0    5252   2688   2560 R 100.0   0.6   0:10.30 gpioread                                             

![StateChart](./images/p1_max_programm.PNG)
CH2: input connected to pin20   
CH1: output connected to pin17

If the CPU us stressed the delay gets bigger:

![StateChart](./images/p1_delay_programm_stress.PNG)
CH2: input connected to pin20   
CH1: output connected to pin17 

## event
    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND                                              
    61132 rpi-sar+  20   0    5256   2816   2688 R  76.8   0.7   0:54.04 gpioevent  
![StateChart](./images/p1_max_event.PNG)
CH2: input connected to pin20   
CH1: output connected to pin17 

If the CPU is stressed there's no difference in the delay.


## interrupt
in module: no measurement for CPU possible

![StateChart](./images/p1_max_interrupt.PNG)
CH2: input connected to pin20   
CH1: output connected to pin17  

If the CPU is stressed there's no difference in the delay.


# Part 3

To enable PWM capabilities:
In /boot/config.txt add following two lines:
    
    dtoverlay=pwm,pin=12,func=2
    dtoverlay=pwm,pin=13,func=2

This should add pwm capabilities for pins 12 and 13

Use sudo nano /boot/config.txt to edit the file.
