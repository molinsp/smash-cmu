@echo off

REM #################################################################
REM # @author James Edmondson <jedmondson@gmail.com>
REM #
REM # This script sets GPS coordinates for a search region. This region is
REM # used by MADARA drone swarms who use the various algorithms in SMASH-CMU
REM # to search an area. Obtaining these variables is done in the following way.
REM #
REM # NORTHERN and WESTERN are the lat and long of the NW most coordinate
REM # SOUTHERN AND EASTERN are the lat and long of the SE most coordinate
REM # DRONES is the number of drones participating in the demo
REM # ID is the id of this process, which should be 1 more than the maximum
REM #      swarm ID. In general, this ID is equal to the number of drones
REM # TYPE is the area coverage type. "random" and "snake" are the likely values.
REM # STRIDE is only used in "snake" and "inside_out" searches, indicates the 
REM #        distance to move when changing to another line in the coverage.
REM # LOG_LEVEL indicates the log level. If not included, not log file will
REM #           be generated.
REM #################################################################

SET NORTHERN=40.441155
SET SOUTHERN=40.440845
SET WESTERN=-79.946861
SET EASTERN=-79.946758
SET DRONES=1
SET ID=203
SET TYPE=snake
SET STRIDE=0.00005
SET LOG_LEVEL=0

cd bin
CommandLineSystemController.exe -i %ID% -d %DRONES% -n %NORTHERN% -s %SOUTHERN% -w %WESTERN% -e %EASTERN% -t %TYPE% -st %STRIDE% -l %LOG_LEVEL%

