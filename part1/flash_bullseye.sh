#!/bin/bash
#  Short script to toggle a GPIO pin at the highest frequency
#  possible using Bash - by Derek Molloy
echo 17 > /sys/class/gpio/export
echo 19 > /sys/class/gpio/export
sleep 0.5
echo "out" > /sys/class/gpio/gpio17/direction
echo "in" > /sys/class/gpio/gpio19/direction
COUNTER=0

while [ $COUNTER -lt 100000 ]; do
    # if cat /sys/class/gpio/gpio19/value 
    value=$(cat /sys/class/gpio/gpio19/value)
    echo $value > /sys/class/gpio/gpio17/value
    let COUNTER=COUNTER+1
    # echo 0 > /sys/class/gpio/gpio17/value
done

echo 17 > /sys/class/gpio/unexport
echo 19 > /sys/class/gpio/unexport


