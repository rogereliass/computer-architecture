@echo off
echo Building simulator3...

if not exist "bin" mkdir bin

gcc -std=c11 -Wall -Wextra src\main.c src\parser.c src\alu.c src\memory.c src\pipeline.c src\utils.c -o simulator3.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo To run with test cases:
    echo simulator3.exe tests\sample1.asm 50
    echo simulator3.exe tests\sample2.asm 50
) else (
    echo Build failed!
) 