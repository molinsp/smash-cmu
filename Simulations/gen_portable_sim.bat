REM Create root folder.
SET PORTABLE_FOLDER=PortableSim
rd /s/q %PORTABLE_FOLDER%
md %PORTABLE_FOLDER%

REM Copy simulated drones.
md %PORTABLE_FOLDER%\Drones
copy SimulationFramework\start_simulated_drones.vbs  %PORTABLE_FOLDER%\Drones\
md %PORTABLE_FOLDER%\Drones\bin
copy SimulationFramework\bin\*.exe  %PORTABLE_FOLDER%\Drones\bin
copy SimulationFramework\bin\*.dll  %PORTABLE_FOLDER%\Drones\bin

REM Add DLLs to drone folder.
copy %MADARA_ROOT%\lib\*.dll   %PORTABLE_FOLDER%\Drones\bin
copy %ACE_ROOT%\lib\*.dll   %PORTABLE_FOLDER%\Drones\bin

REM Copy scene.
md %PORTABLE_FOLDER%\Scene
copy CoverageAndBridgeScenes\* %PORTABLE_FOLDER%\Scene\

REM Copy plugins.
md %PORTABLE_FOLDER%\Scene\Plugins
copy SimulationFramework\lib\*.dll %PORTABLE_FOLDER%\Scene\Plugins

REM Copy Vrep
md %PORTABLE_FOLDER%\V-REP
xcopy /E /EXCLUDE:vrep_exclusions.txt %VREP_HOME%\* %PORTABLE_FOLDER%\V-REP

REM Copy start script.
copy start_sim.bat %PORTABLE_FOLDER%\

pause