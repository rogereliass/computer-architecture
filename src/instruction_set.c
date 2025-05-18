#include <stdio.h>
#include "../includes/instruction_set.h"

// Helper function to update flags according to specifications
static void update_flags(int8_t result, int8_t a, int8_t b, int is_add) {
    // Clear all flags first
    
    // Carry Flag (C) - Check 9th bit (bit 8) of unsigned operation
    if (is_add) {
        int16_t temp = (int16_t)a + (int16_t)b;
        if (temp > INT8_MAX || temp < INT8_MIN) setFlag(CARRY_FLAG);
    }
    
    // Negative Flag (N) - Set if result is negative (bit 7 is 1)
    if (result < 0) setFlag(NEGATIVE_FLAG);
    
    // Zero Flag (Z) - Set if result is zero
    if (result == 0) setFlag(ZERO_FLAG);
    
    // Overflow Flag (V) - For signed operations
    if (is_add) {
        // For addition: V = 1 if both operands have same sign and result has opposite sign
        if ((a > 0 && b > 0 && result < 0) ||
            (a < 0 && b < 0 && result > 0)) {
            setFlag(OVERFLOW_FLAG);
        }
    } else {
        // For subtraction: V = 1 if operands have different signs and result has same sign as subtrahend
        if ((a >= 0 && b < 0 && result < 0) ||
            (a < 0 && b > 0 && result > 0)) {
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
    int8_t a = readRegister(r1);
    int8_t b = readRegister(r2);
    int8_t result = a + b;
    writeRegister(r1, result);
    update_flags(result, a, b, 1);  // 1 indicates addition
    printf("[EX] ADD R%d = R%d + R%d -> %d (0x%02X) (SREG: 0x%02X)\n", 
           r1, r1, r2, result, (uint8_t)result, SREG);
}

void execute_SUB(uint8_t r1, uint8_t r2) {
    int8_t a = readRegister(r1);
    int8_t b = readRegister(r2);
    int8_t result = a - b;
    writeRegister(r1, result);
    update_flags(result, a, b, 0);  // 0 indicates subtraction
    printf("[EX] SUB R%d = R%d - R%d -> %d (0x%02X) (SREG: 0x%02X)\n", 
           r1, r1, r2, result, (uint8_t)result, SREG);
}

void execute_MUL(uint8_t r1, uint8_t r2) {
    int8_t a = readRegister(r1);
    int8_t b = readRegister(r2);
    int16_t result = (int16_t)a * (int16_t)b;

    writeRegister(r1, (int8_t)result);

    if (result < 0) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);

    printf("[EX] MUL R%d = R%d * R%d -> %d (0x%02X) (SREG: 0x%02X)\n", 
           r1, r1, r2, (int8_t)result, (uint8_t)result, SREG);
}

void execute_EOR(uint8_t r1, uint8_t r2) {
    int8_t a = readRegister(r1);
    int8_t b = readRegister(r2);
    int8_t result = a ^ b;
    writeRegister(r1, result);
    // For logical operations, only set N, Z, and S flags
    if (result < 0) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    printf("[EX] EOR R%d = R%d ^ R%d -> %d (0x%02X) (SREG: 0x%02X)\n", 
           r1, r1, r2, result, (uint8_t)result, SREG);
}

void execute_BR(uint8_t r1, uint8_t r2) {
    uint16_t newPC = (readRegister(r1) << 8) | readRegister(r2);
    handleBranchFlush(newPC);
    printf("[EX] BR PC = R%d || R%d -> 0x%04X\n", r1, r2, newPC);
}

// ================== I-Format Instructions ==================

void execute_MOVI(uint8_t r1, int8_t immediate) {
    writeRegister(r1, immediate);  // No need to cast since it's already signed
    printf("[EX] MOVI R%d = %d (0x%02X) (SREG: 0x%02X)\n", r1, immediate, (uint8_t)immediate, SREG);
}

void execute_BEQZ(uint8_t r1, int8_t immediate) {
    if (readRegister(r1) == 0) {
        uint16_t target = PC + 1 + (int16_t)immediate;  // Cast to int16_t for proper signed addition
        handleBranchFlush(target);
        printf("[EX] BEQZ R%d == 0 -> PC = PC + 1 + %d (0x%02X)\n", r1, immediate, (uint8_t)immediate);
    }
}

void execute_ANDI(uint8_t r1, int8_t immediate) {
    int8_t a = readRegister(r1);
    int8_t result = a & immediate;  // No need to cast since it's already signed
    writeRegister(r1, result);
    // For logical operations, only set N, Z, and S flags
    if (result < 0) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    printf("[EX] ANDI R%d = R%d & %d (0x%02X) -> %d (0x%02X) (SREG: 0x%02X)\n", 
           r1, r1, immediate, (uint8_t)immediate, result, (uint8_t)result, SREG);
}

void execute_SAL(uint8_t r1, uint8_t immediate) {
    int8_t a = readRegister(r1);
    int8_t result;
    if (immediate >= 8) {
        result = 0;
    } else {
        result = a << immediate;
    }
    writeRegister(r1, result);
    // For shift operations, only set N, Z, and S flags
    if (result < 0) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    printf("[EX] SAL R%d = R%d << %d -> %d (0x%02X) (SREG: 0x%02X)\n", 
           r1, r1, immediate, result, (uint8_t)result, SREG);
}

void execute_SAR(uint8_t r1, uint8_t immediate) {
    int8_t a = readRegister(r1);
    int8_t result;
    if (immediate >= 8) {
        // Arithmetic shift preserves the sign bit
        result = (a < 0) ? -1 : 0;
    } else {
        // int32_t mask = (~0) << (8 - immediate);
        result = (a >> immediate) ;
    }
    writeRegister(r1, result);
    // For shift operations, only set N, Z, and S flags
    if (result < 0) setFlag(NEGATIVE_FLAG);
    if (result == 0) setFlag(ZERO_FLAG);
    printf("[EX] SAR R%d = R%d >> %d -> %d (0x%02X) (SREG: 0x%02X)\n", 
           r1, r1, immediate, result, (uint8_t)result, SREG);
}

void execute_LDR(uint8_t r1, uint8_t address) {
    uint8_t value = readFromMemory((uint16_t)address, 1);
    writeRegister(r1, (int8_t)value);  // Cast to signed
    printf("[EX] LDR R%d = MEM[0x%02X] -> %d (0x%02X)\n", r1, address, (int8_t)value, value);
}

void execute_STR(uint8_t r1, uint8_t address) {
    int8_t value = readRegister(r1);
    writeToMemory((uint16_t)address, (uint8_t)value, 1);  // Cast to unsigned for memory
    printf("[EX] STR MEM[0x%02X] = R%d -> %d (0x%02X)\n", address, r1, value, (uint8_t)value);
}
