#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include <stdint.h>
#include "registers.h"
#include "memory.h"
#include "control.h"

// R-Format Instructions
void execute_ADD(uint8_t r1, uint8_t r2);
void execute_SUB(uint8_t r1, uint8_t r2);
void execute_MUL(uint8_t r1, uint8_t r2);
void execute_EOR(uint8_t r1, uint8_t r2);
void execute_BR(uint8_t r1, uint8_t r2);

// I-Format Instructions (with signed immediates)
void execute_MOVI(uint8_t r1, int8_t immediate);
void execute_BEQZ(uint8_t r1, int8_t immediate);
void execute_ANDI(uint8_t r1, int8_t immediate);
void execute_SAL(uint8_t r1, uint8_t immediate);
void execute_SAR(uint8_t r1, uint8_t immediate);
void execute_LDR(uint8_t r1, uint8_t address);
void execute_STR(uint8_t r1, uint8_t address);

#endif // INSTRUCTION_SET_H
