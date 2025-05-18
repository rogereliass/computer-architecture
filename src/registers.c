#include <stdio.h>
#include <string.h>
#include "../includes/registers.h"

// Register Definitions
int8_t registers[REGISTER_COUNT];   // General Purpose Registers (signed)
uint8_t SREG = 0x00;                 // Status Register, initialized to 0
uint16_t PC = 0x0000;                // Program Counter, initialized to 0

/**
 * Initializes all registers and the program counter to 0.
 */
void initRegisters() {
    memset(registers, 0, sizeof(registers));
    SREG = 0x00;
    PC = 0x0000;
}

/**
 * Writes a value to a register.
 * @param regNum: The register number (0 to 63).
 * @param value: The 8-bit signed value to write.
 */
void writeRegister(uint8_t regNum, int8_t value) {
    if (regNum < REGISTER_COUNT) {
        registers[regNum] = value;
        printf("[REG] R%d = %d (0x%02X)\n", regNum, value, (uint8_t)value);
    } else {
        printf("Error: Register number %d out of bounds\n", regNum);
    }
}

/**
 * Reads the value of a register.
 * @param regNum: The register number (0 to 63).
 * @return: The 8-bit signed value stored in the register.
 */
int8_t readRegister(uint8_t regNum) {
    if (regNum < REGISTER_COUNT) {
        return registers[regNum];
    } else {
        printf("Error: Register number %d out of bounds\n", regNum);
        return 0;
    }
}

/**
 * Clears all flags in the Status Register (SREG).
 */
void clearAllFlags() {
    // Clear all flags (bits 0-4) while preserving bits 7:5
    SREG &= 0x1F;
}

/**
 * Sets a specific flag in the Status Register (SREG).
 * @param flag: The bit position of the flag to set.
 */
void setFlag(uint8_t flag) {
    if (flag <= ZERO_FLAG) {  // Only allow flags 0-4
        SREG |= (1 << flag);
    }
}

/**
 * Clears a specific flag in the Status Register (SREG).
 * @param flag: The bit position of the flag to clear.
 */
void clearFlag(uint8_t flag) {
    if (flag <= ZERO_FLAG) {  // Only allow flags 0-4
        SREG &= ~(1 << flag);
    }
}

/**
 * Reads the status of a specific flag in the Status Register (SREG).
 * @param flag: The bit position of the flag to read.
 * @return: 1 if the flag is set, 0 otherwise.
 */
uint8_t getFlag(uint8_t flag) {
    if (flag <= ZERO_FLAG) {  // Only allow flags 0-4
        return (SREG >> flag) & 1;
    }
    return 0;
}

/**
 * Increments the Program Counter by 1.
 */
void incrementPC() {
    PC++;
}

/**
 * Sets the Program Counter to a specific address.
 * @param address: The address to set the PC to.
 */
void setPC(uint16_t address) {
    //printf("Setting PC to 0x%04X from %d\n", address, PC);
    PC = address;
}

/**
 * Prints the current state of all registers.
 */
void printRegisterDump() {
    printf("===== Register Dump =====\n");
    for (int i = 0; i < REGISTER_COUNT; ++i) {
        printf("R%d: %d (0x%02X)\n", i, registers[i], (uint8_t)registers[i]);
    }
    printf("\n===== SREG =====\n");
    printf("SREG: 0x%02X (C=%d, V=%d, N=%d, S=%d, Z=%d)\n", 
           SREG, getFlag(CARRY_FLAG), getFlag(OVERFLOW_FLAG), 
           getFlag(NEGATIVE_FLAG), getFlag(SIGN_FLAG), getFlag(ZERO_FLAG));
    printf("\n===== Program Counter =====\n");
    printf("PC: 0x%04X\n\n", PC);
}
