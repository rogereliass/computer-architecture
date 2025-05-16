#include <stdio.h>
#include <string.h>
#include "../includes/memory.h"

// Memory Arrays
uint16_t instructionMemory[INSTRUCTION_MEMORY_SIZE];
uint8_t dataMemory[DATA_MEMORY_SIZE];

/**
 * Initializes the Instruction and Data memory to zero.
 */
void initMemory() {
    memset(instructionMemory, 0, sizeof(instructionMemory));
    memset(dataMemory, 0, sizeof(dataMemory));
}

/**
 * Writes a value to memory.
 * @param address: The memory address to write to.
 * @param value: The value to write.
 * @param isDataMemory: 1 if writing to Data Memory, 0 if writing to Instruction Memory.
 */
void writeToMemory(uint16_t address, uint16_t value, int isDataMemory) {
    if (isDataMemory) {
        if (address < DATA_MEMORY_SIZE) {
            dataMemory[address] = (uint8_t)value;
            printf("[MEM] Data Memory [0x%04X] = 0x%02X\n", address, value);
        } else {
            printf("Error: Data Memory Address out of bounds\n");
        }
    } else {
        if (address < INSTRUCTION_MEMORY_SIZE) {
            instructionMemory[address] = value;
            printf("[MEM] Instruction Memory [0x%04X] = 0x%04X\n", address, value);
        } else {
            printf("Error: Instruction Memory Address out of bounds\n");
        }
    }
}

/**
 * Reads a value from memory.
 * @param address: The memory address to read from.
 * @param isDataMemory: 1 if reading from Data Memory, 0 if reading from Instruction Memory.
 * @return: The value read from memory.
 */
uint16_t readFromMemory(uint16_t address, int isDataMemory) {
    if (isDataMemory) {
        if (address < DATA_MEMORY_SIZE) {
            return dataMemory[address];
        } else {
            printf("Error: Data Memory Address out of bounds\n");
            return 0;
        }
    } else {
        if (address < INSTRUCTION_MEMORY_SIZE) {
            return instructionMemory[address];
        } else {
            printf("Error: Instruction Memory Address out of bounds\n");
            return 0;
        }
    }
}

/**
 * Prints a memory dump for debugging.
 */
void printMemoryDump() {
    printf("===== Instruction Memory Dump =====\n");
    for (int i = 0; i < INSTRUCTION_MEMORY_SIZE; ++i) {
        if (instructionMemory[i] != 0) {
            printf("Addr [%d] : 0x%04X\n", i, instructionMemory[i]);
        }
    }
    
    printf("\n===== Data Memory Dump =====\n");
    for (int i = 0; i < DATA_MEMORY_SIZE; ++i) {
        if (dataMemory[i] != 0) {
            printf("Addr [%d] : 0x%02X\n", i, dataMemory[i]);
        }
    }
    printf("\n");
}
