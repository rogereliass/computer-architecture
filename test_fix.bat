@echo off
echo Building simulator...
gcc -std=c11 -Wall -Wextra src\main.c src\parser.c src\alu.c src\memory.c src\pipeline.c src\utils.c -o simulator3.exe

if %ERRORLEVEL% NEQ 0 (
  echo Build failed!
  exit /b 1
)

echo.
echo Running sample2.asm with cycle limit...
echo.
simulator3.exe tests\sample2.asm 100

echo.
echo If you see "SIMULATION COMPLETE" the fix worked.
echo If you see "SIMULATION STOPPED: MAXIMUM CYCLES" the simulator still has issues. 