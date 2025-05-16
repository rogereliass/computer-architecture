#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>   // For uint8_t and uint16_t types

// Register Sizes
#define REGISTER_COUNT 64

// SREG Flag Positions (Bit Positions)
#define CARRY_FLAG     0  // Carry Flag
#define OVERFLOW_FLAG  1  // Overflow Flag
#define NEGATIVE_FLAG  2  // Negative Flag
#define SIGN_FLAG      3  // Sign Flag
#define ZERO_FLAG      4  // Zero Flag

// Register Declarations
extern uint8_t registers[REGISTER_COUNT];  // 64 General Purpose Registers
extern uint8_t SREG;                       // Status Register (8 bits)
extern uint16_t PC;                        // Program Counter (16 bits)

// Function Prototypes
void initRegisters();
void writeRegister(uint8_t regNum, uint8_t value);
uint8_t readRegister(uint8_t regNum);
void setFlag(uint8_t flag);
void clearFlag(uint8_t flag);
uint8_t getFlag(uint8_t flag);
void incrementPC();
void setPC(uint16_t address);
void printRegisterDump();

// Flag handling methods
void clearAllFlags();

#endif // REGISTERS_H
