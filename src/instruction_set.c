#include <stdio.h>
#include "../includes/instruction_set.h"

// Helper function to update flags according to specifications
static void update_flags(uint8_t result, uint8_t a, uint8_t b, int is_add) {
    // Clear all flags first
    clearAllFlags();
    
    // Carry Flag (C) - Check 9th bit (bit 8) of unsigned operation
    if (is_add) {
        uint16_t temp = (uint16_t)a + (uint16_t)b;
        if (temp > 0xFF) setFlag(CARRY_FLAG);
    }
    
    // Negative Flag (N) - Set if result is negative (bit 7 is 1)
    if (result & 0x80) setFlag(NEGATIVE_FLAG);
    
    // Zero Flag (Z) - Set if result is zero
    if (result == 0) setFlag(ZERO_FLAG);
    
    // Overflow Flag (V) - For signed operations
    if (is_add) {
        // For addition: V = 1 if both operands have same sign and result has opposite sign
        int8_t signed_a = (int8_t)a;
        int8_t signed_b = (int8_t)b;
        int8_t signed_result = (int8_t)result;
        if ((signed_a > 0 && signed_b > 0 && signed_result < 0) ||
            (signed_a < 0 && signed_b < 0 && signed_result > 0)) {
            setFlag(OVERFLOW_FLAG);
        }
    } else {
        // For subtraction: V = 1 if operands have different signs and result has same sign as subtrahend
        int8_t signed_a = (int8_t)a;
        int8_t signed_b = (int8_t)b;
        int8_t signed_result = (int8_t)result;
        if ((signed_a >= 0 && signed_b < 0 && signed_result < 0) ||
            (signed_a < 0 && signed_b > 0 && signed_result > 0)) {
            setFlag(OVERFLOW_FLAG);
        }
    }
    
    // Sign Flag (S) - S = N ⊕ V
    if (getFlag(NEGATIVE_FLAG) ^ getFlag(OVERFLOW_FLAG)) {
        setFlag(SIGN_FLAG);
    }
}

// ================== R-Format Instructions ==================

void execute_ADD(uint8_t r1, uint8_t r2) {
    uint8_t a = readRegister(r1);
    uint8_t b = readRegister(r2);
    uint8_t result = a + b;
    writeRegister(r1, result);
    update_flags(result, a, b, 1);  // 1 indicates addition
    printf("[EX] ADD R%d = R%d + R%d -> 0x%02X (SREG: 0x%02X)\n", 
           r1, r1, r2, result, SREG);
}

void execute_SUB(uint8_t r1, uint8_t r2) {
    uint8_t a = readRegister(r1);
    uint8_t b = readRegister(r2);
    uint8_t result = a - b;
    writeRegister(r1, result);
    update_flags(result, a, b, 0);  // 0 indicates subtraction
    printf("[EX] SUB R%d = R%d - R%d -> 0x%02X (SREG: 0x%02X)\n", 
           r1, r1, r2, result, SREG);
}

void execute_MUL(uint8_t r1, uint8_t r2) {
    uint8_t a = readRegister(r1);
    uint8_t b = readRegister(r2);
    uint8_t result = a * b;
    writeRegister(r1, result);
    // For multiplication, only set N, Z, and S flags
    clearAllFlags();
    if (result & 0x80) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    // S = N (since V is always 0 for multiplication)
    if (result & 0x80) setFlag(SIGN_FLAG);
    printf("[EX] MUL R%d = R%d * R%d -> 0x%02X (SREG: 0x%02X)\n", 
           r1, r1, r2, result, SREG);
}

void execute_EOR(uint8_t r1, uint8_t r2) {
    uint8_t a = readRegister(r1);
    uint8_t b = readRegister(r2);
    uint8_t result = a ^ b;
    writeRegister(r1, result);
    // For logical operations, only set N, Z, and S flags
    clearAllFlags();
    if (result & 0x80) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    // S = N (since V is always 0 for logical operations)
    if (result & 0x80) setFlag(SIGN_FLAG);
    printf("[EX] EOR R%d = R%d ^ R%d -> 0x%02X (SREG: 0x%02X)\n", 
           r1, r1, r2, result, SREG);
}

void execute_BR(uint8_t r1, uint8_t r2) {
    uint16_t newPC = (readRegister(r1) << 8) | readRegister(r2);
    handleBranchFlush(newPC);
    printf("[EX] BR PC = R%d || R%d -> 0x%04X\n", r1, r2, newPC);
}

// ================== I-Format Instructions ==================

void execute_MOVI(uint8_t r1, uint8_t immediate) {
    writeRegister(r1, immediate);
    // For immediate load, only set N, Z, and S flags
    clearAllFlags();
    if (immediate & 0x80) setFlag(NEGATIVE_FLAG);
    if (immediate == 0) setFlag(ZERO_FLAG);
    // S = N (since V is always 0 for immediate load)
    if (immediate & 0x80) setFlag(SIGN_FLAG);
    printf("[EX] MOVI R%d = 0x%02X (SREG: 0x%02X)\n", r1, immediate, SREG);
}

void execute_BEQZ(uint8_t r1, uint8_t immediate) {
    if (readRegister(r1) == 0) {
        uint16_t target = PC + 1 + immediate;
        handleBranchFlush(target);
        printf("[EX] BEQZ R%d == 0 -> PC = PC + 1 + 0x%02X\n", r1, immediate);
    }
}

void execute_ANDI(uint8_t r1, uint8_t immediate) {
    uint8_t a = readRegister(r1);
    uint8_t result = a & immediate;
    writeRegister(r1, result);
    // For logical operations, only set N, Z, and S flags
    clearAllFlags();
    if (result & 0x80) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    // S = N (since V is always 0 for logical operations)
    if (result & 0x80) setFlag(SIGN_FLAG);
    printf("[EX] ANDI R%d = R%d & 0x%02X -> 0x%02X (SREG: 0x%02X)\n", 
           r1, r1, immediate, result, SREG);
}

void execute_SAL(uint8_t r1, uint8_t immediate) {
    uint8_t a = readRegister(r1);
    uint8_t result;
    if (immediate >= 8) {
        result = 0;
    } else {
        result = a << immediate;
    }
    writeRegister(r1, result);
    // For shift operations, only set N, Z, and S flags
    clearAllFlags();
    if (result & 0x80) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    // S = N (since V is always 0 for shift operations)
    if (result & 0x80) setFlag(SIGN_FLAG);
    printf("[EX] SAL R%d = R%d << %d -> 0x%02X (SREG: 0x%02X)\n", 
           r1, r1, immediate, result, SREG);
}

void execute_SAR(uint8_t r1, uint8_t immediate) {
    uint8_t a = readRegister(r1);
    uint8_t result;
    if (immediate >= 8) {
        // Arithmetic shift preserves the sign bit
        result = (a & 0x80) ? 0xFF : 0x00;
    } else {
        // Implement arithmetic right shift (preserve sign bit)
        int8_t signed_a = (int8_t)a;
        int8_t signed_res = signed_a >> immediate;
        result = (uint8_t)signed_res;
    }
    writeRegister(r1, result);
    // For shift operations, only set N, Z, and S flags
    clearAllFlags();
    if (result & 0x80) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    // S = N (since V is always 0 for shift operations)
    if (result & 0x80) setFlag(SIGN_FLAG);
    printf("[EX] SAR R%d = R%d >> %d -> 0x%02X (SREG: 0x%02X)\n", 
           r1, r1, immediate, result, SREG);
}

void execute_LDR(uint8_t r1, uint8_t address) {
    uint8_t value = readFromMemory(address, 1);
    writeRegister(r1, value);
    printf("[EX] LDR R%d = MEM[0x%02X] -> 0x%02X\n", r1, address, value);
}

void execute_STR(uint8_t r1, uint8_t address) {
    uint8_t value = readRegister(r1);
    writeToMemory(address, value, 1);
    printf("[EX] STR MEM[0x%02X] = R%d -> 0x%02X\n", address, r1, value);
}
