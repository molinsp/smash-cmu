@ECHO OFF
:loop
  cls
  netsh interface ip show joins
  timeout /t 2
goto loop