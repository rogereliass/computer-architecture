#ifndef ALU_H
#define ALU_H
#include <stdint.h>

uint8_t alu_add(uint8_t a, uint8_t b);
uint8_t alu_sub(uint8_t a, uint8_t b);
uint8_t alu_mul(uint8_t a, uint8_t b);
uint8_t alu_andi(uint8_t a, uint8_t imm);
uint8_t alu_eor(uint8_t a, uint8_t b);
uint8_t alu_sal(uint8_t a, uint8_t imm);
uint8_t alu_sar(uint8_t a, uint8_t imm);

#endif 