#include "alu.h"
#include "utils.h"
#include <stdio.h>

static void update_flags(uint8_t result, int has_carry, int carry, int overflow) {
    // Clear all flags first
    SREG &= ~(FLAG_C | FLAG_V | FLAG_N | FLAG_Z | FLAG_S);
    
    // Set flags as needed
    if (has_carry && carry)
        SREG |= FLAG_C;
        
    if (overflow)
        SREG |= FLAG_V;
        
    if (result & 0x80)
        SREG |= FLAG_N;
        
    if (result == 0)
        SREG |= FLAG_Z;
        
    // S = N⊕V
    if (((SREG & FLAG_N) >> 2) ^ ((SREG & FLAG_V) >> 1))
        SREG |= FLAG_S;
}

uint8_t alu_add(uint8_t a, uint8_t b) {
    uint16_t result = (uint16_t)a + (uint16_t)b;
    uint8_t res8 = (uint8_t)result;
    
    // Carry: Set if there's a carry from bit 7
    int carry = result > 0xFF;
    
    // Overflow: Set if the sign bit changes unexpectedly
    int overflow = (~(a ^ b) & (a ^ res8) & 0x80) != 0;
    
    update_flags(res8, 1, carry, overflow);
    return res8;
}

uint8_t alu_sub(uint8_t a, uint8_t b) {
    uint8_t res8 = a - b;
    
    // Overflow: Set if the sign bit changes unexpectedly
    int overflow = ((a ^ b) & (a ^ res8) & 0x80) != 0;
    
    update_flags(res8, 0, 0, overflow);
    return res8;
}

uint8_t alu_mul(uint8_t a, uint8_t b) {
    uint8_t res8 = a * b;
    
    // Multiplication doesn't set carry or overflow
    update_flags(res8, 0, 0, 0);
    return res8;
}

uint8_t alu_andi(uint8_t a, uint8_t imm) {
    uint8_t res8 = a & imm;
    
    // Logical operations don't set carry or overflow
    update_flags(res8, 0, 0, 0);
    return res8;
}

uint8_t alu_eor(uint8_t a, uint8_t b) {
    uint8_t res8 = a ^ b;
    
    // Logical operations don't set carry or overflow
    update_flags(res8, 0, 0, 0);
    return res8;
}

uint8_t alu_sal(uint8_t a, uint8_t imm) {
    uint8_t res8;
    
    if (imm >= 8) {
        res8 = 0;
    } else {
        res8 = a << imm;
    }
    
    // Shift operations don't set carry or overflow
    update_flags(res8, 0, 0, 0);
    return res8;
}

uint8_t alu_sar(uint8_t a, uint8_t imm) {
    uint8_t res8;
    
    if (imm >= 8) {
        // Arithmetic shift preserves the sign bit
        res8 = (a & 0x80) ? 0xFF : 0x00;
    } else {
        // Implement arithmetic right shift (preserve sign bit)
        int8_t signed_a = (int8_t)a;
        int8_t signed_res = signed_a >> imm;
        res8 = (uint8_t)signed_res;
    }
    
    // Shift operations don't set carry or overflow
    update_flags(res8, 0, 0, 0);
    return res8;
} 