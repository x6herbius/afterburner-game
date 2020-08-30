@echo off

REM WE do this because PowerShell doesn't have the `ENV=var mycmd` syntax that Bash has...
setlocal
  set NOCLIMB=true && python waf %*
endlocal
