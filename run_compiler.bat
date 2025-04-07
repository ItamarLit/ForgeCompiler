@echo off
echo Running compiler...

if "%~1"=="" (
    echo Usage: run_compiler.bat file.forge [-T] [-A] [-S] [-O]
    exit /b 1
)

set INPUT=ProjectData\%1
shift
set FLAGS=%*

Debug\ForgeCompilerProj.exe %INPUT% %FLAGS%
echo Compiler finished with errorlevel=%errorlevel%

if %errorlevel% neq 0 (
    exit /b %errorlevel%
)

IF EXIST ProjectData\output.asm (
    echo Assembling...
    ml64 /c /Fo ProjectData\output.obj ProjectData\output.asm

    echo Linking...
    link ProjectData\output.obj /entry:Main /subsystem:console kernel32.lib /OUT:ProjectData\output.exe

    IF EXIST ProjectData\output.exe (
        echo Running program...
        ProjectData\output.exe
        del ProjectData\output.obj
    ) ELSE (
        echo Linking failed.
    )
) 