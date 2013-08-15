#!/bin/sh
SLEEP=0.1
if [ $# = 0 ]
then
    echo "usage: $0 <NUM_DRONES> <SLEEP:0.1>"
    echo "Launches <NUM_DRONES> with <SLEEP> between launches"
else
    for i in $(seq 1 1 $1)
    do
        id=`expr $i - 1`
        output="output$id"
        rm -f $output
        DroneControllerSimulator/DroneControllerSimulator -i $id >> $output 2>> $output &
        sleep $SLEEP
    done
fi
