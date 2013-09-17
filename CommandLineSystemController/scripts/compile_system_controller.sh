#!/bin/bash

#################################################################
# @author James Edmondson <jedmondson@gmail.com>
# this script will compile the command line system controller. Be sure
# to set the following environment variables:
#
# $SMASH_ROOT   		this is the location of the SMASH repository
# $MADARA_ROOT          this is the location of your MADARA installation dir
# $ACE_ROOT             this is the location of your ACE installation dir
################################################################

SMASH_ROOT=~/smash-cmu
MADARA_CLIENT_ROOT=$SMASH_ROOT/DroneController/packages/madara_client

cd $SMASH_ROOT/CommandLineSystemController
g++ -I$MADARA_ROOT/include -I$ACE_ROOT -I$MADARA_CLIENT_ROOT -L$MADARA_ROOT/lib -L$ACE_ROOT/lib simple_coverage_test.cpp $MADARA_CLIENT_ROOT/platforms/comm/kb_setup.cpp $MADARA_CLIENT_ROOT/platforms/comm/multicast/comm_multicast.cpp -lMADARA -lACE -o simple_coverage_test

