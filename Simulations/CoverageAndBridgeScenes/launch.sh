#!/bin/bash
SCENE_DIR=`pwd`
V_REP_HOME=$HOME/bin/V-REP_PRO_EDU_V3_0_4_64_Linux
FRAMEWORK_DIR=$SCENE_DIR/../../SimulationFramework

# delete previous log
rm -f vrep_output.log

# create symlinks
cd $V_REP_HOME
rm -f LaptopController.lua QuadricopterTargetController.lua Utils.lua
ln -s $SCENE_DIR/LaptopController.lua
ln -s $SCENE_DIR/QuadricopterTargetController.lua
ln -s $SCENE_DIR/Utils.lua

# launch vrep
rm -f $SCENE_DIR/vrep_output.txt
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$V_REP_HOME
export LD_LIBRARY_PATH
./vrep $SCENE_DIR/area_coverage_and_bridge_building.ttt >> $SCENE_DIR/vrep_output.log 2>> $SCENE_DIR/vrep_output.log &
