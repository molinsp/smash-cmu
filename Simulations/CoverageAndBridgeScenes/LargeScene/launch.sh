#!/bin/bash
SCENE_DIR=`pwd`
V_REP_HOME=$HOME/bin/V-REP_PRO_EDU_V3_0_4_64_Linux
FRAMEWORK_DIR=$SCENE_DIR/../../SimulationFramework

# create random location files if necessary
if [ ! -e "$FRAMEWORK_DIR/Tools/GenerateRandomLocations" ]
then
    make -C $FRAMEWORK_DIR
fi

if [ ! -e "BillLocation.lua" ]
then
    $FRAMEWORK_DIR/Tools/GenerateRandomLocations -m g_billLocs -i 20 -w -90 -e 0 -n 90 -s 0 > BillLocations.lua
fi

if [ ! -e "QuadricopterLocations.lua" ]
then
    $FRAMEWORK_DIR/Tools/GenerateRandomLocations -m g_droneLocs -i 20 -w -90 -e 0 -n 90 -s 0 > QuadricopterLocations.lua
fi

# create symlinks
cd $V_REP_HOME
rm -f BillController.lua BillLocations.lua FloorController.lua LaptopController.lua Params.lua QuadricopterController.lua QuadricopterLocations.lua Utils.lua
ln -s $SCENE_DIR/BillController.lua
ln -s $SCENE_DIR/BillLocations.lua
ln -s $SCENE_DIR/FloorController.lua
ln -s $SCENE_DIR/LaptopController.lua
ln -s $SCENE_DIR/Params.lua
ln -s $SCENE_DIR/QuadricopterController.lua
ln -s $SCENE_DIR/QuadricopterLocations.lua
ln -s $SCENE_DIR/Utils.lua

# launch vrep
rm -f vrep_output.txt
./vrep.sh $SCENE_DIR/large_area_coverage.ttt >> $SCENE_DIR/vrep_output.txt 2>> $SCENE_DIR/vrep_output.txt &
