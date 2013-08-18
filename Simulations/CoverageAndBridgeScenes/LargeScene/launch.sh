#!/bin/bash
SCENE_DIR=`pwd`
V_REP_HOME=$HOME/bin/V-REP_PRO_EDU_V3_0_4_64_Linux
FRAMEWORK_DIR=$SCENE_DIR/../../SimulationFramework

# check args
if [ $# != 1 ]
then
    echo "usage: $0 <LOCATIONS_SET>"
    echo "Launches vrep using the LOCATIONS_SET"
    exit -1
fi
if [ ! -e "$1/IndependentParams.lua" ]
then
    echo "$1/IndependentParams.lua does not exist"
    exit -1
fi
if [ ! -e "$1/BillLocations.lua" ]
then
    echo "$1/BillLocations.lua does not exist"
    exit -1
fi
if [ ! -e "$1/QuadricopterLocations.lua" ]
then
    echo "$1/QuadricopterLocations.lua does not exist"
    exit -1
fi

# delete previous log
rm -f vrep_output.log

# create symlinks
cd $V_REP_HOME
rm -f BillController.lua BillLocations.lua FloorController.lua LaptopController.lua Params.lua IndependentParams.lua QuadricopterController.lua QuadricopterLocations.lua Utils.lua
ln -s $SCENE_DIR/BillController.lua
ln -s $SCENE_DIR/$1/BillLocations.lua
ln -s $SCENE_DIR/FloorController.lua
ln -s $SCENE_DIR/LaptopController.lua
ln -s $SCENE_DIR/Params.lua
ln -s $SCENE_DIR/$1/IndependentParams.lua
ln -s $SCENE_DIR/QuadricopterController.lua
ln -s $SCENE_DIR/$1/QuadricopterLocations.lua
ln -s $SCENE_DIR/Utils.lua

# launch vrep
rm -f $SCENE_DIR/vrep_output.txt
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$V_REP_HOME
export LD_LIBRARY_PATH
./vrep $SCENE_DIR/large_area_coverage.ttt >> $SCENE_DIR/vrep_output.log 2>> $SCENE_DIR/vrep_output.log &
