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
# COMM_RANGE is the max distance (in meters) that two drones can be without
#            losing contact, important for bridge building.
# PERSON_LAT and PERSON_LON are the lat and long of a person we want to build
#            a bridge towards (it can be just a source position).
# SINK_LAT and SINK_LON are the lat and long of a device that wants to receive
#            communication from a formed bridge (the sink position).
# 
#################################################################

#SMASH_ROOT=~/smash-cmu

NORTHERN=40.441155
SOUTHERN=40.440845
WESTERN=-79.946861
EASTERN=-79.946758
DRONES=1
ID=203
TYPE=random
STRIDE=0.00001
LOG_LEVEL=0

# All coordinates to zero will disable bridge building.
COMM_RANGE=15
PERSON_LAT=0
PERSON_LON=0
SINK_LAT=0
SINK_LON=0


cd $SMASH_ROOT/CommandLineSystemController
./simple_coverage_test -i $ID -d $DRONES -n $NORTHERN -s $SOUTHERN -w $WESTERN -e $EASTERN -t $TYPE -st STRIDE -l $LOG_LEVEL -bplat $PERSON_LAT -bplon $PERSON_LON -bslat $SINK_LAT -bslon $SINK_LON -r $COMM_RANGE

