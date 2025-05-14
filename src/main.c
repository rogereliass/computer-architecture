#include "memory.h"
#include "parser.h"
#include "pipeline.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// Function to print the final state of the processor
void dump_processor_state() {
    int i;
    
    // Dump registers
    printf("\n==== REGISTER DUMP ====\n");
    for (i = 0; i < 64; i += 4) {
        printf("R%-2d: 0x%02X  R%-2d: 0x%02X  R%-2d: 0x%02X  R%-2d: 0x%02X\n", 
               i, (i == 0) ? 0 : GPR[i],         // R0 is always 0
               i+1, GPR[i+1], 
               i+2, GPR[i+2], 
               i+3, GPR[i+3]);
    }
    
    // Dump status register
    printf("\n==== STATUS REGISTER ====\n");
    printf("SREG: 0x%02X (", SREG);
    printf("%c", (SREG & FLAG_C) ? 'C' : '-');
    printf("%c", (SREG & FLAG_V) ? 'V' : '-');
    printf("%c", (SREG & FLAG_N) ? 'N' : '-');
    printf("%c", (SREG & FLAG_S) ? 'S' : '-');
    printf("%c", (SREG & FLAG_Z) ? 'Z' : '-');
    printf(")\n");
    
    // Dump program counter
    printf("\n==== PROGRAM COUNTER ====\n");
    printf("PC: 0x%04X\n", PC);
    
    // Dump instruction memory
    printf("\n==== INSTRUCTION MEMORY DUMP ====\n");
    for (i = 0; i < 1024; i++) {
        if (instr_mem[i] != 0 && instr_mem[i] != NOP) {
            printf("INSTR_MEM[0x%04X]: 0x%04X\n", i, instr_mem[i]);
        }
    }
    
    // Dump data memory
    printf("\n==== DATA MEMORY DUMP ====\n");
    int empty_line = 1;
    for (i = 0; i < 2048; i++) {
        if (data_mem[i] != 0) {
            printf("DATA_MEM[0x%04X]: 0x%02X\n", i, data_mem[i]);
            empty_line = 0;
        }
    }
    
    if (empty_line) {
        printf("(Empty)\n");
    }
}

int main(int argc, char **argv) {
    // Check command line arguments
    if (argc < 2) {
        printf("Usage: %s <assembly_file> [max_cycles]\n", argv[0]);
        return 1;
    }
    
    // Parse max cycles if provided
    int max_cycles = 100; // Default to 100 cycles as safety
    if (argc >= 3) {
        max_cycles = atoi(argv[2]);
        if (max_cycles <= 0) {
            printf("Invalid max_cycles value. Using default (100).\n");
            max_cycles = 100;
        }
    }
    printf("Using max_cycles: %d\n", max_cycles);
    
    // Initialize pipeline registers
    IF_ID if_id = {NOP, 0};
    ID_EX id_ex = {NOP, 0, 0, 0, 0, 0};
    EX_WB ex_wb = {NOP, 0, 0, 0, 0};
    
    // Load the program
    load_program(argv[1]);
    
    // Initialize control signals
    int fetch_allowed = 1;
    int flush = 0;
    int cycle = 1;
    
    // Main simulation loop
    while ((fetch_allowed || if_id.ir != NOP || id_ex.ir != NOP || ex_wb.ir != NOP) &&
           (cycle <= max_cycles)) {
        printf("\n[Cycle %d]\n", cycle);
        
        // Execute pipeline stages in reverse order
        writeback_stage(&ex_wb);
        execute_stage(&id_ex, &ex_wb, &flush, &fetch_allowed);
        decode_stage(&if_id, &id_ex);
        fetch_stage(&if_id, &fetch_allowed);
        
        // Handle branch/jump flushes
        if (flush) {
            if_id.ir = NOP;
            id_ex.ir = NOP;
            flush = 0;
        }
        
        cycle++;
    }
    
    // Check if terminated due to max cycles
    if (cycle > max_cycles) {
        printf("\n==== SIMULATION STOPPED: MAXIMUM CYCLES (%d) REACHED ====\n", max_cycles);
    } else {
        printf("\n==== SIMULATION COMPLETE ====\n");
    }
    printf("Total cycles executed: %d\n", cycle - 1);
    
    // Dump final processor state
    dump_processor_state();
    
    return 0;
} 