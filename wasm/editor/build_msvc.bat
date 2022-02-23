@echo off

echo AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
set CompilerParams= -MT -nologo -Gm- -GR- -Od -Oi -W4 -wd4201 -wd4100 -wd4244 -wd4189 -DQUOTE_INTERNAL=1 -DQUOTE_SLOW=1
REM set CompilerParams = /MT /nologo /O2 /Ot /fp:fast /arch:AVX2 /Gv /GL 
set CommonLinkerFlags= /LTCG -incremental:no -opt:ref user32.lib Gdi32.lib Xinput.lib Winmm.lib
set Compiler= "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30037\bin\Hostx64\x64\cl.exe"
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

%Compiler% /O2 /LD quote.cpp /link /EXPORT:GameUpdateAndRender %CommonLinkerFlags%
%Compiler% /O2 win32.cpp /link %CommonLinkerFlags%

echo AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

%1
pause
