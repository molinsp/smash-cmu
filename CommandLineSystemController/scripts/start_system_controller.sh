#!/bin/bash

#################################################################
# @author James Edmondson <jedmondson@gmail.com>
#
# This script sets GPS coordinates for a search region. This region is
# used by MADARA drone swarms who use the various algorithms in SMASH-CMU
# to search an area. Obtaining these variables is done in the following way.
#
# NORTHERN and WESTERN are the lat and long of the NW most coordinate
# SOUTHERN AND EASTERN are the lat and long of the SE most coordinate
# DRONES is the number of drones participating in the demo
# ID is the id of this process, which should be 1 more than the maximum
#      swarm ID. In general, this ID is equal to the number of drones
# TYPE is the area coverage type. "random" and "snake" are the likely values.
# STRIDE is only used in "snake" and "inside_out" searches, indicates the 
#        distance to move when changing to another line in the coverage.
# LOG_LEVEL indicates the log level. If not included, not log file will
#           be generated.
#################################################################


NORTHERN=40.441155
SOUTHERN=40.440845
WESTERN=-79.946861
EASTERN=-79.946758
DRONES=1
ID=203
TYPE=snake
STRIDE=0.00005
LOG_LEVEL=0


~/smash-cmu/CommandLineSystemController/simple_coverage_test -i $ID -d $DRONES -n $NORTHERN -s $SOUTHERN -w $WESTERN -e $EASTERN -t $TYPE -st STRIDE -l $LOG_LEVEL

