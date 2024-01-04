@echo off
echo building game to dll

REM Move the pdb out because it's probably locked by the debugger.
mkdir platform_win32\LockedAssemblies
del /S /Q platform_win32\LockedAssemblies\*
move platform_win32\game.pdb platform_win32\LockedAssemblies\game%random%.pdb

REM delete the old dll
del "platform_win32\game.dll"

REM build a new dll
clang -g -O0 -mrdrnd -gcodeview -shared raven/game.c -o platform_win32/game.dll > output.txt 2>&1
REM clang -Ofast -mrdrnd -shared raven/game.c -o platform_win32/game.dll > output.txt 2>&1

set "ERROR=%errorlevel%"

REM Read out the error log
call findstr Compiling output.txt
echo [33m
call findstr warning output.txt
echo [31m
call findstr error output.txt
echo [0m

del output.txt
IF %ERROR%==0 (
	echo COMPILE SUCCESS
) ELSE (
	echo [31mCOMPILE FAILED[0m
	pause
)