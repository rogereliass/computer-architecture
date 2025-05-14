#include "memory.h"
#include <stdio.h>

// Memory arrays
uint16_t instr_mem[1024] = {0};
uint8_t data_mem[2048] = {0};

uint16_t load_instr(uint16_t addr) {
    if (addr < 1024) {
        return instr_mem[addr];
    }
    printf("ERROR: Instruction memory access out of bounds: %u\n", addr);
    return 0xFFFF; // Return NOP
}

uint8_t load_data(uint16_t addr) {
    if (addr < 2048) {
        return data_mem[addr];
    }
    printf("ERROR: Data memory read out of bounds: %u\n", addr);
    return 0;
}

void store_data(uint16_t addr, uint8_t val) {
    if (addr < 2048) {
        printf("MEM[%u] ← 0x%02X\n", addr, val);
        data_mem[addr] = val;
    } else {
        printf("ERROR: Data memory write out of bounds: %u\n", addr);
    }
} 