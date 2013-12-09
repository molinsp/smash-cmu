' Get the main WMI object.
Set objWMIService = GetObject("winmgmts:\\.\root\cimv2")
sCurPath = CreateObject("Scripting.FileSystemObject").GetAbsolutePathName(".")

' Create a file to store the PIDs of the drones, so we can close them later.
'Set objFSO=CreateObject("Scripting.FileSystemObject")
'pidFile = sCurPath & "\dronepids.txt"
'Set objFile = objFSO.CreateTextFile(pidFile,True)

' Setup objects to create the process, and to pass it a configuration (for the window to start in a certain place).
Set objNewProcess = objWMIService.Get("Win32_Process")
Set objConfig = objWMIService.Get("Win32_ProcessStartup")
objConfig.SpawnInstance_

' Setup the command line to start the drones.
exePath = sCurPath  & "\bin"
startCommand = "cmd.exe /K DroneControllerSimulator.exe -l 1 -i "

' Start all drones.
objConfig.X = 100
objConfig.Y = 10
intReturn = objNewProcess.Create(startCommand & 0, exePath, objConfig, intProcessID)
'objFile.Write intProcessID & vbCrLf

objConfig.X = 800
objConfig.Y = 10
intReturn = objNewProcess.Create(startCommand & 1, exePath, objConfig, intProcessID)
'objFile.Write intProcessID & vbCrLf

objConfig.X = 100
objConfig.Y = 600
intReturn = objNewProcess.Create(startCommand & 2, exePath, objConfig, intProcessID)
'objFile.Write intProcessID & vbCrLf

objConfig.X = 800
objConfig.Y = 600
intReturn = objNewProcess.Create(startCommand & 3, exePath, objConfig, intProcessID)
'objFile.Write intProcessID & vbCrLf

' Close the PID file.
'objFile.Close
