#!/bin/sh
xterm -hold -e DroneControllerSimulator/DroneControllerSimulator -i 0 &
sleep 1.5
xterm -hold -e DroneControllerSimulator/DroneControllerSimulator -i 1 &
sleep 1.5
xterm -hold -e DroneControllerSimulator/DroneControllerSimulator -i 2 &
sleep 1.5
xterm -hold -e DroneControllerSimulator/DroneControllerSimulator -i 3 &
