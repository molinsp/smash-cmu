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

SET DRONES=4
SET ID=203
SET LOG_LEVEL=0

SET NORTHERN=40.44116098380300200000
SET SOUTHERN=40.44107999999999900000
SET WESTERN=-79.94717580322499400000
SET EASTERN=-79.94706957420099500000 
SET TYPE=snake
SET STRIDE=0.00001

SET COMM_RANGE=4
SET PERSON_LAT=40.44115941
SET PERSON_LON=-79.9471755
SET SINK_LAT=40.44108653
SET SINK_LON=-79.94711443

cd bin
REM CommandLineSystemController.exe -i %ID% -d %DRONES% -n %NORTHERN% -s %SOUTHERN% -w %WESTERN% -e %EASTERN% -t %TYPE% -st %STRIDE% -l %LOG_LEVEL%
CommandLineSystemController.exe -i %ID% -d %DRONES% -n %NORTHERN% -s %SOUTHERN% -w %WESTERN% -e %EASTERN% -t %TYPE% -st %STRIDE% -bplat %PERSON_LAT% -bplon %PERSON_LON% -bslat %SINK_LAT% -bslon %SINK_LON% -l %LOG_LEVEL% -r %COMM_RANGE%

pause