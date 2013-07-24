#!/bin/sh
if [ $# != 2 ]
then
    echo "usage: $0 <NUM_DRONES> <SLEEP>"
    echo "Launches <NUM_DRONES> with <SLEEP> between launches"
else
    for i in $(seq 1 1 $1)
    do
        id=`expr $i - 1`
        xterm -hold -e DroneControllerSimulator/DroneControllerSimulator -i $id &
        sleep $2
    done
fi
