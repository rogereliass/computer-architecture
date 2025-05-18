#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h> // For uint8_t and uint16_t types

// Memory Sizes
#define INSTRUCTION_MEMORY_SIZE 1024    // 1024 words (16 bits each)
#define DATA_MEMORY_SIZE 2048           // 2048 bytes (8 bits each)

// Memory Arrays
extern uint16_t instructionMemory[INSTRUCTION_MEMORY_SIZE]; // 16-bit instruction memory
extern int8_t dataMemory[DATA_MEMORY_SIZE];                // 8-bit data memory

// Function Prototypes
void initMemory();
void writeToMemory(uint16_t address, uint16_t value, int isDataMemory);
uint16_t readFromMemory(uint16_t address, int isDataMemory);
void printMemoryDump();

#endif // MEMORY_H

