@echo off
cls
echo %path%

echo building win32 platform layer to exe

del "platform_win32\win32.exe"

REM build the exe
clang -g -O0 -gcodeview Platform_Win32/win32.c -o Platform_Win32/win32.exe -Wl,user32.lib,Gdi32.lib

REM call win32_hotreload.bat

REM start the program
REM "Platform_Win32\win32.exe"
