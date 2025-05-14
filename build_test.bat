@echo off
echo Building simulator...
gcc -std=c11 -Wall -Wextra src\main.c src\parser.c src\alu.c src\memory.c src\pipeline.c src\utils.c -o simulator3.exe
echo Done. 