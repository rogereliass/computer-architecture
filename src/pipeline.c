#include "pipeline.h"
#include "memory.h"
#include "utils.h"
#include "alu.h"
#include <stdio.h>

// Helper function to get instruction mnemonic
static const char* get_mnemonic(uint16_t ir) {
    if (ir == NOP) return "NOP";
    
    int opcode = (ir >> 12) & 0x0F;
    
    switch (opcode) {
        case 0: return "ADD";
        case 1: return "SUB";
        case 2: return "MUL";
        case 3: return "ANDI";
        case 4: return "EOR";
        case 5: return "SAL";
        case 6: return "SAR";
        case 7: return "LOAD";
        case 8: return "STORE";
        case 9: return "BEQZ";
        case 10: return "JR";
        default: return "UNKNOWN";
    }
}

// Helper function to extract register indices
static void extract_registers(uint16_t ir, uint8_t *rd, uint8_t *rs1, uint8_t *rs2, int16_t *imm) {
    int opcode = (ir >> 12) & 0x0F;
    
    // Extract fields based on instruction format
    if (ir == NOP) {
        *rd = *rs1 = *rs2 = 0;
        *imm = 0;
        return;
    }
    
    // Default extraction for each field
    *rd = (ir >> 6) & 0x3F;
    *rs1 = ir & 0x3F;
    *rs2 = 0;
    *imm = (int8_t)(ir & 0xFF); // sign-extend when used as immediate
    
    // R-type instructions
    if (opcode <= 2 || opcode == 4) {
        // ADD, SUB, MUL, EOR
        *rd = (ir >> 6) & 0x3F;
        *rs1 = ir & 0x3F;
        *rs2 = (ir >> 0) & 0x3F;
        *imm = 0;
    }
    // I-type with rd as destination (ANDI, SAL, SAR, LOAD)
    else if (opcode == 3 || opcode == 5 || opcode == 6 || opcode == 7) {
        *rd = (ir >> 6) & 0x3F;
        *rs1 = ir & 0x3F;
        *rs2 = 0;
        *imm = (int8_t)(ir & 0xFF);
    }
    // STORE instruction
    else if (opcode == 8) {
        *rd = 0;
        *rs1 = (ir >> 6) & 0x3F;
        *rs2 = ir & 0x3F;
        *imm = (int8_t)(ir & 0xFF);
    }
    // BEQZ instruction
    else if (opcode == 9) {
        *rd = 0;
        *rs1 = (ir >> 6) & 0x3F;
        *rs2 = 0;
        *imm = (int8_t)(ir & 0xFF);
    }
    // JR instruction
    else if (opcode == 10) {
        *rd = 0;
        *rs1 = (ir >> 6) & 0x3F;
        *rs2 = ir & 0x3F;
        *imm = 0;
    }
}

// Instruction Fetch Stage
void fetch_stage(IF_ID *if_id, int *fetch_allowed) {
    if (*fetch_allowed && PC < 1024) {
        if_id->ir = load_instr(PC);
        if_id->pc_if = PC;
        PC++;
        
        printf("IF: %s (0x%04X)\n", get_mnemonic(if_id->ir), if_id->ir);
    } else {
        if_id->ir = NOP;
        if_id->pc_if = PC;
        printf("IF: IDLE\n");
    }
}

// Instruction Decode Stage
void decode_stage(const IF_ID *if_id, ID_EX *id_ex) {
    id_ex->ir = if_id->ir;
    id_ex->pc_if = if_id->pc_if;
    
    extract_registers(if_id->ir, &id_ex->rd, &id_ex->rs1, &id_ex->rs2, &id_ex->imm);
    
    const char* mnemonic = get_mnemonic(if_id->ir);
    
    if (if_id->ir == NOP) {
        printf("ID: NOP\n");
    } else {
        int opcode = (if_id->ir >> 12) & 0x0F;
        
        // Different formats for different instruction types
        if (opcode <= 2 || opcode == 4) {
            // R-type
            printf("ID: %s R%d, R%d, R%d\n", mnemonic, id_ex->rd, id_ex->rs1, id_ex->rs2);
        } else if (opcode == 3 || opcode == 5 || opcode == 6) {
            // I-type
            printf("ID: %s R%d, R%d, %d\n", mnemonic, id_ex->rd, id_ex->rs1, id_ex->imm);
        } else if (opcode == 7) {
            // LOAD
            printf("ID: %s R%d, R%d, %d\n", mnemonic, id_ex->rd, id_ex->rs1, id_ex->imm);
        } else if (opcode == 8) {
            // STORE
            printf("ID: %s R%d, R%d, %d\n", mnemonic, id_ex->rs1, id_ex->rs2, id_ex->imm);
        } else if (opcode == 9) {
            // BEQZ
            printf("ID: %s R%d, %d\n", mnemonic, id_ex->rs1, id_ex->imm);
        } else if (opcode == 10) {
            // JR
            printf("ID: %s R%d, R%d\n", mnemonic, id_ex->rs1, id_ex->rs2);
        }
    }
}

// Execution Stage (also handles memory access and writeback)
void execute_stage(const ID_EX *id_ex, EX_WB *ex_wb, int *flush, int *fetch_allowed) {
    ex_wb->ir = id_ex->ir;
    ex_wb->rd = id_ex->rd;
    ex_wb->memory_op = 0;
    ex_wb->mem_addr = 0;
    ex_wb->result = 0;
    
    if (id_ex->ir == NOP) {
        printf("EX: NOP\n");
        printf("WB: NOP\n");
        return;
    }
    
    int opcode = (id_ex->ir >> 12) & 0x0F;
    
    // Execute based on opcode
    switch (opcode) {
        case 0: // ADD
            ex_wb->result = alu_add(GPR[id_ex->rs1], GPR[id_ex->rs2]);
            printf("EX: ADD R%d, R%d, R%d = 0x%02X\n", id_ex->rd, id_ex->rs1, id_ex->rs2, ex_wb->result);
            break;
            
        case 1: // SUB
            ex_wb->result = alu_sub(GPR[id_ex->rs1], GPR[id_ex->rs2]);
            printf("EX: SUB R%d, R%d, R%d = 0x%02X\n", id_ex->rd, id_ex->rs1, id_ex->rs2, ex_wb->result);
            break;
            
        case 2: // MUL
            ex_wb->result = alu_mul(GPR[id_ex->rs1], GPR[id_ex->rs2]);
            printf("EX: MUL R%d, R%d, R%d = 0x%02X\n", id_ex->rd, id_ex->rs1, id_ex->rs2, ex_wb->result);
            break;
            
        case 3: // ANDI
            ex_wb->result = alu_andi(GPR[id_ex->rs1], id_ex->imm);
            printf("EX: ANDI R%d, R%d, %d = 0x%02X\n", id_ex->rd, id_ex->rs1, id_ex->imm, ex_wb->result);
            break;
            
        case 4: // EOR
            ex_wb->result = alu_eor(GPR[id_ex->rs1], GPR[id_ex->rs2]);
            printf("EX: EOR R%d, R%d, R%d = 0x%02X\n", id_ex->rd, id_ex->rs1, id_ex->rs2, ex_wb->result);
            break;
            
        case 5: // SAL
            ex_wb->result = alu_sal(GPR[id_ex->rs1], id_ex->imm);
            printf("EX: SAL R%d, R%d, %d = 0x%02X\n", id_ex->rd, id_ex->rs1, id_ex->imm, ex_wb->result);
            break;
            
        case 6: // SAR
            ex_wb->result = alu_sar(GPR[id_ex->rs1], id_ex->imm);
            printf("EX: SAR R%d, R%d, %d = 0x%02X\n", id_ex->rd, id_ex->rs1, id_ex->imm, ex_wb->result);
            break;
            
        case 7: // LOAD
            ex_wb->mem_addr = GPR[id_ex->rs1] + id_ex->imm;
            ex_wb->result = load_data(ex_wb->mem_addr);
            ex_wb->memory_op = 1; // Load operation
            printf("EX: LOAD R%d, [R%d + %d] = 0x%02X (addr 0x%04X)\n", 
                   id_ex->rd, id_ex->rs1, id_ex->imm, ex_wb->result, ex_wb->mem_addr);
            break;
            
        case 8: // STORE
            ex_wb->mem_addr = GPR[id_ex->rs1] + id_ex->imm;
            ex_wb->result = GPR[id_ex->rs2];
            ex_wb->memory_op = 2; // Store operation
            printf("EX: STORE [R%d + %d], R%d = 0x%02X (addr 0x%04X)\n", 
                   id_ex->rs1, id_ex->imm, id_ex->rs2, ex_wb->result, ex_wb->mem_addr);
            
            // Perform the store in EX stage
            store_data(ex_wb->mem_addr, ex_wb->result);
            break;
            
        case 9: // BEQZ
            printf("EX: BEQZ R%d, %d (R%d = 0x%02X)\n", 
                   id_ex->rs1, id_ex->imm, id_ex->rs1, GPR[id_ex->rs1]);
            
            // Branch if R[rs1] is zero
            if (GPR[id_ex->rs1] == 0) {
                PC = id_ex->pc_if + 1 + id_ex->imm;
                *flush = 1;
                printf("    Branch taken: PC ← 0x%04X\n", PC);
            } else {
                printf("    Branch not taken\n");
            }
            break;
            
        case 10: // JR
            // Print register values for debugging
            printf("EX: JR R%d, R%d (R%d=0x%02X, R%d=0x%02X)\n", 
                   id_ex->rs1, id_ex->rs2, id_ex->rs1, GPR[id_ex->rs1], id_ex->rs2, GPR[id_ex->rs2]);
            
            // Jump to address formed by rs1 (high byte) and rs2 (low byte)
            PC = (GPR[id_ex->rs1] << 8) | GPR[id_ex->rs2];
            *flush = 1;
            printf("    Jump target: PC ← 0x%04X\n", PC);
            break;
            
        default:
            printf("EX: UNKNOWN instruction (0x%04X)\n", id_ex->ir);
            break;
    }
    
    // Writeback happens in the writeback_stage function which is called after this
    printf("WB: %s", get_mnemonic(ex_wb->ir));
    
    // Check if we're at the end of the program
    if (PC >= 1024 || load_instr(PC) == NOP) {
        *fetch_allowed = 0;
    }
}

// Writeback Stage
void writeback_stage(const EX_WB *ex_wb) {
    if (ex_wb->ir == NOP) {
        return; // Already printed in execute_stage
    }
    
    int opcode = (ex_wb->ir >> 12) & 0x0F;
    
    // Writeback to register file for most instructions
    if (opcode <= 7) { // ADD, SUB, MUL, ANDI, EOR, SAL, SAR, LOAD
        if (ex_wb->rd == 0) {
            printf(" (attempted write to R0, ignored)\n");
        } else {
            GPR[ex_wb->rd] = ex_wb->result;
            printf(" (R%d ← 0x%02X)\n", ex_wb->rd, ex_wb->result);
        }
    } else if (opcode == 8) { // STORE
        // Store already done in execute_stage
        printf("\n");
    } else {
        // Other instructions don't do writeback
        printf("\n");
    }
} 