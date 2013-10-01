Set objWMIService = GetObject("winmgmts:\\.\root\cimv2")
Set objNewProcess = objWMIService.Get("Win32_Process")

Set objConfig = objWMIService.Get("Win32_ProcessStartup")
objConfig.SpawnInstance_

startCommand = "cmd.exe /K DroneControllerSimulator.exe -l 1 -i "
sCurPath = CreateObject("Scripting.FileSystemObject").GetAbsolutePathName(".")
exePath = sCurPath  & "\bin"

objConfig.X = 100
objConfig.Y = 10
intReturn = objNewProcess.Create(startCommand & 0, exePath, objConfig, intProcessID)

objConfig.X = 800
objConfig.Y = 10
intReturn = objNewProcess.Create(startCommand & 1, exePath, objConfig, intProcessID)

objConfig.X = 100
objConfig.Y = 600
intReturn = objNewProcess.Create(startCommand & 2, exePath, objConfig, intProcessID)

objConfig.X = 800
objConfig.Y = 600
intReturn = objNewProcess.Create(startCommand & 3, exePath, objConfig, intProcessID)