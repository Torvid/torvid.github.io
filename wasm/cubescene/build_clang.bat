@echo off

REM clang -Ofast -Wl,user32.lib -Wl,Gdi32.lib -Wl,Xinput.lib -Wl,Winmm.lib -o win32.exe win32.cpp

clang -march=native -Ofast -shared -o quote.dll quote.cpp


REM clang -o win32.exe win32.cpp
REM "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\User32.Lib"
REM "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\Gdi32.Lib"
REM "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\xinput.lib"
REM "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\WinMM.Lib" 

REM SET LIBRARY_PATH=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\

REM C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30037\lib\x64\
REM 
REM SET PATH=%PATH%;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\
REM SET PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30037\lib\x64\



REM clang -O3 -march=native -flto -Wl,-O3 -target x86_64-w64-mingw32 -c -shared -v -o quote.dll quote.cpp


REM clang -Wl,user32.lib -Wl,Gdi32.lib -Wl,Xinput.lib -Wl,Winmm.lib -o win32.exe win32.cpp
REM clang -c -shared -v -o quote.dll quote.cpp

REM clang++ -c -o quote.o quote.cpp
REM clang++ -shared -v -o quote.dll quote.o



REM clang -c -o quote.o quote.cpp

REM lld-link /SUBSYSTEM:WINDOWS -lc quote.o 

REM lld-link /SUBSYSTEM:WINDOWS user32.lib Gdi32.lib Xinput.lib Winmm.lib quote.o
