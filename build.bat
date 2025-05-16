@echo off
setlocal

REM Check if mingw32-make is available
where mingw32-make >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: mingw32-make not found in PATH
    echo Please install MinGW and add it to your PATH
    echo You can download MinGW from: https://sourceforge.net/projects/mingw/
    echo After installation, add the MinGW bin directory to your PATH
    echo Typical path: C:\MinGW\bin
    pause
    exit /b 1
)

REM If no arguments provided, show help
if "%~1"=="" (
    echo Usage: build.bat [command]
    echo Commands:
    echo   build  - Build the processor executable
    echo   run    - Build and run the processor
    echo   clean  - Remove all build files
    echo   help   - Show this help message
    exit /b 0
)

REM Execute the requested command
if "%~1"=="build" (
    mingw32-make all
) else if "%~1"=="run" (
    mingw32-make run
) else if "%~1"=="clean" (
    mingw32-make clean
) else if "%~1"=="help" (
    mingw32-make help
) else (
    echo Unknown command: %~1
    echo Use 'build.bat help' to see available commands
    exit /b 1
)

endlocal 