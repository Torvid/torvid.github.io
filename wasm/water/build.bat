
@echo off
echo %1

REM If we should compile just the dll and not the exe.
set dll_only=false

REM If we should move up one folder.
set up_dir=false

if "%1" == "dll_only" set dll_only=true
if "%2" == "dll_only" set dll_only=true
if "%3" == "dll_only" set dll_only=true
if "%1" == "up_dir" set up_dir=true
if "%2" == "up_dir" set up_dir=true
if "%3" == "up_dir" set up_dir=true

IF "%up_dir%"=="true" (
	cd ..
)

REM Move the pdb out because it might be locked by visual studio's shitty debugger for no reason.
del /S /Q LockedAssemblies\*
move quote.pdb LockedAssemblies\quote%random%.pdb

cls
del *.pdb
del *.qir
cls

REM Empty the text file
break>win32_output.txt
break>quote_output.txt

IF "%dll_only%"=="false" (
	"C:\Program Files\LLVM\bin\clang" -g -O0 -gcodeview -mavx2 -Wno-everything -march=native -o win32.exe win32.cpp -Wl,user32.lib -Wl,Gdi32.lib -Wl,Xinput.lib -Wl,Winmm.lib -Wl,Dxva2.lib -Wl,Rstrtmgr.lib -Wl,dinput8.lib -Wl,dxguid.lib
)

set clang_optimize=false

	echo Compiling quote.cpp into quote.dll >> quote_output.txt
IF "%clang_optimize%"=="true" (
	"C:\Program Files\LLVM\bin\clang" -g -gcodeview -mavx2 -Ofast -Wno-everything -march=native -shared -o quote.dll quote.cpp
) ELSE (
	"C:\Program Files\LLVM\bin\clang" -g -O0 -gcodeview -mavx2 -Wno-everything -march=native -shared -o quote.dll quote.cpp
)

REM save the error number form the compiler
set "ERROR=%errorlevel%"

REM Read out the error log
call findstr Compiling win32_output.txt
echo [33m
call findstr warning win32_output.txt
echo [31m
call findstr error win32_output.txt
echo [0m

call findstr Compiling quote_output.txt
echo [33m
call findstr warning quote_output.txt
echo [31m
call findstr error quote_output.txt
echo [0m

IF %ERROR%==0 (
	echo COMPILE SUCCESS
) ELSE (
	echo [31mCOMPILE FAILED[0m
	pause
)

exit /b 0

:end