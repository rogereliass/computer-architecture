#include "../includes/pipeline.h"
#include "../includes/memory.h"
#include "../includes/registers.h"
#include "../includes/parser.h"
#include <stdio.h>

int main() {
    // Initialize system components
    initMemory();
    initRegisters();
    initPipeline();

    // Create a test program file
    FILE* testFile = fopen("test_program.txt", "w");
    if (!testFile) {
        printf("Error: Could not create test program file\n");
        return 1;
    }

    // Write test instructions to file
    fprintf(testFile, "MOVI R1 10\n");
    fprintf(testFile, "MOVI R2 5\n");
    fprintf(testFile, "MOVI R3 -3\n");
    fprintf(testFile, "MOVI R4 2\n");
    fprintf(testFile, "MOVI R5 0\n");

    fprintf(testFile, "ADD R1 R2\n");
    fprintf(testFile, "SUB R1 R3\n");
    fprintf(testFile, "MUL R1 R4\n");

    fprintf(testFile, "ANDI R1 15\n");
    fprintf(testFile, "EOR R1 R2\n");

    fprintf(testFile, "BEQZ R5 3\n");
    fprintf(testFile, "MOVI R1 30\n");
    fprintf(testFile, "MOVI R2 30\n");
    fprintf(testFile, "MOVI R3 30\n");
    fprintf(testFile, "MOVI R6 -8\n");

    // fprintf(testFile, "MOVI R9 1\n");
    // fprintf(testFile, "MOVI R10 1\n");
    // fprintf(testFile, "BR R9 R10\n");

    fprintf(testFile, "SAL R6 1\n");
    fprintf(testFile, "SAR R6 2\n");

    fprintf(testFile, "MOVI R7 31\n");
    fprintf(testFile, "STR R7 50\n");
    fprintf(testFile, "LDR R8 50\n");

    // fprintf(testFile, "MOVI R1 31\n");
    // fprintf(testFile, "MOVI R2 1\n");
    // fprintf(testFile, "MUL R1 R2\n");
    fclose(testFile);

    // Parse and load the program
    printf("\n=== Loading Program ===\n");
    int instructionCount = parseInstructionFile("test_program.txt");
    if (instructionCount < 0) {
        printf("Error: Failed to parse program\n");
        return 1;
    }
    printf("Successfully loaded %d instructions\n", instructionCount);

    // Set initial register values for testing
    // printf("\n=== Setting Initial Register Values ===\n");
    // writeRegister(1, 0);   // R1 = 0
    // writeRegister(2, 9);  // R2 = 20
    // setFlag(ZERO_FLAG);
    // writeRegister(3, 30);  // R3 = 30
    // writeRegister(4, 0);   // R4 = 0 (for BEQZ test)
    // writeRegister(5, 8);   // R5 = 8 (for shift test)
    // writeRegister(6, -8);  // R6 = -8 (for arithmetic shift test)
    // writeRegister(7, 0);   // R7 = 0 (for load test)
    // writeRegister(8, 42);  // R8 = 42 (for store test)
    // writeRegister(9, 10);  // R9 = 10 (for branch test)
    // writeRegister(10, 1); // R10 = 10 (for branch test)
    // writeRegister(11, 2); // R11 = 10 (for branch test)
    // writeRegister(12, 3); // R12 = 10 (for branch test)
    // writeRegister(13, 4); // R13 = 10 (for branch test)
    // writeRegister(14, 8); // R14 = 10 (for branch test)
    // writeRegister(15, 7); // R15 = 10 (for branch test)
    // writeRegister(16, 6); // R16 = 10 (for branch test)
    // writeRegister(17, 5); // R17 = 10 (for branch test)
    

    // Set some initial memory values
    printf("\n=== Setting Initial Memory Values ===\n");
    writeToMemory(50, 0x55, 1);  // Data memory[10] = 0x55
    // writeToMemory(20, 0x00, 1);  // Data memory[20] = 0x00

    // Print initial state
    printf("\n=== Initial State ===\n");
    printRegisterDump();
    printMemoryDump();

    // Run the pipeline for a few cycles
    printf("\n=== Running Pipeline ===\n");
    while(pipelineCycle());

    // Print final state
    printf("\n=== Final State ===\n");
    printRegisterDump();
    printMemoryDump();

    return 0;
}
