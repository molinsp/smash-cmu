#!/bin/bash
SCENE_DIR=`pwd`
V_REP_HOME=$HOME/V-REP_PRO_EDU_V3_0_4_64_Linux
FRAMEWORK_DIR=$SCENE_DIR/../../SimulationFramework

# delete previous log
rm -f vrep_output.log

# create symlinks
cd $V_REP_HOME
rm -f SystemController.lua MinimapControl.lua QuadricopterTargetController.lua Utils.lua
ln -s $SCENE_DIR/SystemController.lua $V_REP_HOME
ln -s $SCENE_DIR/MinimapControl.lua $V_REP_HOME
ln -s $SCENE_DIR/QuadricopterTargetController.lua $V_REP_HOME
ln -s $SCENE_DIR/Utils.lua $V_REP_HOME

# launch vrep
rm -f $SCENE_DIR/vrep_output.txt
LD_LIBRARY_PATH=$V_REP_HOME:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
./vrep $SCENE_DIR/area_coverage_and_bridge_building.ttt >> $SCENE_DIR/vrep_output.log 2>> $SCENE_DIR/vrep_output.log &
