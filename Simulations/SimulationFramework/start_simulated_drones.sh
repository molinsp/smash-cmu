#!/bin/sh
SLEEP=0.1
if [ $# = 0 ]
then
    echo "usage: $0 -i <NUM_DRONES> <SLEEP:0.1>"
    echo "Launches <NUM_DRONES> with <SLEEP> between launches"
else
    for i in $(seq 1 1 $1)
    do
        id=`expr $i - 1`
        xterm -hold -l -e ./bin/DroneControllerSimulator -i $id &
        sleep $SLEEP
    done
fi
