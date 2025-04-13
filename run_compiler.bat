@echo off
echo Running compiler...

if "%~1"=="" (
    echo Usage: run_compiler.bat file.forge [-T] [-A] [-S] [-O]
    exit /b 1
)

if /i not "%~x1"==".forge" (
    echo Error: Input file must have a .forge extension.
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

set ASMFILE=%INPUT:.forge=.asm%

IF EXIST %ASMFILE% (
    echo Assembling...
    ml64 /c /Fo "%ASMFILE:.asm=.obj%" "%ASMFILE%"

    echo Linking...
    link "%ASMFILE:.asm=.obj%" /entry:Main /subsystem:console kernel32.lib /OUT:"%ASMFILE:.asm=.exe%"

    IF EXIST "%ASMFILE:.asm=.exe%" (
        echo Running program...
        "%ASMFILE:.asm=.exe%"
        del "%ASMFILE:.asm=.obj%"
    ) ELSE (
        echo Linking failed.
    )
) 