
COUNTER=0
# LED blinking
# while [ $COUNTER -lt 100000 ]; do
#     # gpioset --mode=time --mode=time --sec=3 gpiochip0 17=1 # not working -- no glue why
#     gpioset 0 17=1
#     sleep 1s
#     gpioset 0 17=0
#     sleep 1s
# done
# gpioset 0 20=0
gpioset 0 17=0
while [ $COUNTER -lt 100000 ]; do
    # gpioset 0 17=0
    # Block until an edge event occurs. Don't print anything.
    # gpiomon --num-events=1 --silent --rising-edge 20 0 # not working -- no glue why
    if [[ $(gpioget 0 20) == "1" ]]; then
        gpioset 0 17=1
    fi
        if [[ $(gpioget 0 20) == "0" ]]; then
        gpioset 0 17=0
    fi
done


# FROM DOCUMENTAION
#  # Blink an LED on GPIO22 at 1Hz
# gpioset -t500ms GPIO22=1

# # Blink an LED on GPIO22 at 1Hz with a 20% duty cycle
# gpioset -t200ms,800ms GPIO22=1