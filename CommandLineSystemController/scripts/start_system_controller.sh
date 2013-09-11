#!/bin/bash


NORTHERN=40.441155
SOUTHERN=40.440845
WESTERN=-79.946861
EASTERN=-79.946758
DRONES=1
ID=1
TYPE=snake


~/smash-cmu/CommandLineSystemController/simple_coverage_test -i $ID -d $DRONES -n $NORTHERN -s $SOUTHERN -w $WESTERN -e $EASTERN -t $TYPE

