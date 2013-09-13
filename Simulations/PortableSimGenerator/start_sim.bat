cd Scene
SET VREP_HOME=..\V-REP
copy .\Plugins\*.dll "%VREP_HOME%"
start cmd /K start_coverage_sim.bat
cd ..
cd Drones
start_simulated_drones.vbs