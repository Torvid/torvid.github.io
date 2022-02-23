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
if "%1" == "use_clang" set use_clang=true
if "%2" == "use_clang" set use_clang=true
if "%3" == "use_clang" set use_clang=true

IF "%up_dir%"=="true" (
	cd ..
)

REM mode con: cols=200 lines=30

set CompilerParams= -MT -nologo -Gm- -GR- -Od -Oi -W4 -wd4201 -wd4100 -wd4244 -wd4189 -DQUOTE_INTERNAL=1 -DQUOTE_SLOW=1 /Zi 
REM set CompilerParams= -MT -nologo -Gm- -GR- -Oi -W4 -wd4201 -wd4100 -wd4244 -wd4189 -DQUOTE_INTERNAL=1 -DQUOTE_SLOW=1 /O2 /fp:fast
set CommonLinkerFlags= /LTCG -incremental:no -opt:ref user32.lib Gdi32.lib Xinput.lib Winmm.lib Dxva2.lib
set Compiler= "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30037\bin\Hostx64\x64\cl.exe"

call "VCVarsFast.bat"
cls
del *.pdb
del *.qir
cls
REM Empty the text file
break>win32_output.txt
break>quote_output.txt

IF "%dll_only%"=="false" (
	echo Compiling win32.cpp into win32.exe >> win32_output.txt
	%Compiler% %CompilerParams% win32.cpp /link %CommonLinkerFlags% >> win32_output.txt
)

set use_clang=true

IF "%use_clang%"=="true" (

	echo Compiling quote.cpp into quote.dll >> quote_output.txt
	"C:\Program Files\LLVM\bin\clang" -Wno-everything -march=native -Ofast -gcodeview -shared -o quote.dll quote.cpp
	REM "C:\Program Files\LLVM\bin\clang" -g -O0 -gcodeview -Wno-everything -march=native -gcodeview -shared -o quote.dll quote.cpp
	
) ELSE (

	echo Compiling quote.cpp into quote.dll >> quote_output.txt
	%Compiler% %CompilerParams% /LD quote.cpp /link /PDB:quote_%RANDOM%.pdb -EXPORT:GameUpdateAndRender >> quote_output.txt
	
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
