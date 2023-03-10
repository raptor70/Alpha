@echo off
echo %~dp0
@rem if exist %~dp0\cache rmdir %~dp0\cache /s /q

for /r %~dp0\ %%f in (*.ttf *.tif *.png) do (
echo Cache %%f
call %~dp0\game.exe CACHE %%f
)

pause