#!/bin/bash 2>nul || goto :windows

# linux + osx -----------------------------------------------------------------
cd `dirname $0`

# tidy environment
if [ "$1" = "tidy" ]; then
    rm a.out 2> /dev/null
    rm engine.o 2> /dev/null
    exit
fi

# sync
if [ "$1" = "sync" ]; then
    git reset --hard HEAD^1 && git pull
    sh MAKE.bat tidy
    exit
fi

export args=
export cc=cc
export o=-o

if [ "$(uname)" != "Darwin" ]; then
fi

if [ "$(uname)" = "Darwin" ]; then
fi

!cc! !o! hello.exe hello.c -I code code/engine.c !args! || exit 1

exit


:: -----------------------------------------------------------------------------
:windows

@echo off
cd /d "%~dp0"

if "%1"=="ext" (
    rem if exist code\ext\ext-fmod\x64\*.dll   copy code\ext\ext-fmod\x64\*.dll   > nul
    rem if exist code\ext\ext-assimp\x64\*.dll copy code\ext\ext-assimp\x64\*.dll > nul
    call ext\MAKE.bat
    exit /b
)

rem tidy environment
if "%1"=="tidy" (
    del *.def                       > nul 2> nul
    del *.mem                       > nul 2> nul
    del *.exp                       > nul 2> nul
    del *.lib                       > nul 2> nul
    del *.exe                       > nul 2> nul
    del *.obj                       > nul 2> nul
    del *.o                         > nul 2> nul
    del *.a                         > nul 2> nul
    del *.pdb                       > nul 2> nul
    del *.ilk                       > nul 2> nul
    rd /q /s .vs                    > nul 2> nul
    del cook*.csv                   > nul 2> nul
    del *.dll                       > nul 2> nul
    del .settings.ini               > nul 2> nul
    del .log.txt                    > nul 2> nul
    del ";*"                        > nul 2> nul
    rd /q /s lib                    > nul 2> nul
    exit /b
)

rem sync
if "%1"=="sync" (
    git reset --hard HEAD~1 && git pull
    call MAKE.bat tidy
    exit /b
)

rem Compiler detection
if "%cc%"=="" (
    echo Detecting VS 2022/2019/2017/2015/2013 x64 ...
    (set "cc=cl" && where /q cl.exe) || (

               if exist "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%ProgramFiles%/microsoft visual studio/2022/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles%/microsoft visual studio/2022/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2019/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2017/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" (
                  @call "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" amd64 >nul && set "vs=15"
        ) else if exist "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" (
                  @call "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" amd64 >nul && set "vs=13"
        ) else (
            echo Detecting Mingw64 ...
            (set "cc=gcc" && where /q gcc.exe) || (
                echo Detecting TCC ... && set "cc=tcc"
            )
        )
    )
)

if "%cc%"=="cl" (
    set o=/Fe:
    set args=/Zi /MT /EHsc /std:c++17 /nologo
) else (
    set o=-o
    set args=
)

setlocal enableDelayedExpansion

rem PAUSE only if double-clicked from Windows explorer
rem (((echo.%cmdcmdline%)|%WINDIR%\system32\find.exe /I "%~0")>nul)&&pause
set "needs_pause=0"
(((echo .%cmdcmdline% | find /i "/c")>nul) && set "needs_pause=1")
rem (((echo .%cmdcmdline% | find /i "visual studio")>nul) && set "needs_pause=1")
(((echo .%cmdcmdline% | find /i "VsDevCmd")>nul) && set "needs_pause=0")
rem If /I "%COMSPEC%" == "%CMDCMDLINE%" set needs_pause=0

:parse_args
if not "%1"=="" (
    if exist "%1" (
        set objs=!objs! %1
    ) else (
        set args=!args! %1
    )
    shift
    goto parse_args
)
if "!objs!"=="" (
    set objs=!o! hello.exe hello.c
)

if not exist "SDL3.dll" copy /y code\3rd\SDL3\x64 > nul
pushd ext & call make & popd

rem collect include folders iteratively
for /D %%i in (ext\*) do if exist "%%i\inc"     set exts=!exts! -I%%i\inc
for /D %%i in (ext\*) do if exist "%%i\include" set exts=!exts! -I%%i\include
for /D %%i in (ext\*) do if exist "%%i\x64"     set libs=!libs! /LIBPATH:%%i\x64
set args=!exts! !args!

rem since it takes a lot of time, compile SDL3.cc at least once a day. hopefully ok for most users.
rem get system date and remove '/' chars on it. also remove spaces present on some configs.
set datestr=%date%
set datestr=%datestr:/=%
set datestr=%datestr:-=%
set datestr=%datestr:.=%
set datestr=%datestr: =%
if not exist SDL3-!datestr!.obj (
echo vs%vs%: !cc! code\sdl3.cc /Fo: SDL3-!datestr!.obj -I code -I code/3rd /c !args!
             !cc! code\sdl3.cc /Fo: SDL3-!datestr!.obj -I code -I code/3rd /c !args! || set rc=1
)

if not exist deps-!datestr!.obj (
echo vs%vs%: !cc! code\3rd\3rd.c /Fo: deps-!datestr!.obj -I code -I code/3rd /c !args!
             !cc! code\3rd\3rd.c /Fo: deps-!datestr!.obj -I code -I code/3rd /c !args! || set rc=1
)

if not exist lua-!datestr!.obj (
echo vs%vs%: !cc! code\3rd\lua.c /Fo: lua-!datestr!.obj -I code -I code/3rd /c !args!
             !cc! code\3rd\lua.c /Fo: lua-!datestr!.obj -I code -I code/3rd /c !args! || set rc=1
)

echo vs%vs%: !cc! !objs! code/engine.c -I code -I code/3rd SDL3-!datestr!.obj deps-!datestr!.obj lua-!datestr!.obj !args! /link !libs!
             !cc! !objs! code/engine.c -I code -I code/3rd SDL3-!datestr!.obj deps-!datestr!.obj lua-!datestr!.obj !args! /link !libs! || set rc=1

:eof
if "!needs_pause!"=="1" pause

cmd /c exit !rc!
