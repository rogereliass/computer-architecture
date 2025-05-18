#include <stdio.h>
#include "../includes/pipeline.h"

// ================== Pipeline Register Definitions ==================
IF_ID_Reg IF_ID;
ID_EX_Reg ID_EX;

// Cycle Counter and Halt Flag
static int cycle = 0;
static bool isHalted = false;
bool isStalled = false;

/**
 * Initializes the pipeline registers.
 */
void initPipeline() {
    IF_ID.instruction = 0;
    IF_ID.nextPC = 0;
    IF_ID.valid = false;

    ID_EX.opcode = 0;
    ID_EX.r1 = 0;
    ID_EX.r2 = 0;
    ID_EX.isImmediate = false;
    ID_EX.nextPC = 0;
    ID_EX.valid = false;

    cycle = 0;
    isHalted = false;
}

/**
 * Instruction Fetch (IF) Stage
 */
void fetchStage() {
    if (isHalted) return;

    if (PC < INSTRUCTION_MEMORY_SIZE) {
        uint16_t instruction = readFromMemory(PC, 0);

        // Detect HALT Instruction (0xFFFF)
        if (instruction == 0xFFFF) {
            printf("[HALT] Halt instruction detected. Pipeline will drain...\n");
            isHalted = true;
            IF_ID.valid = false;
            return;
        }

        IF_ID.instruction = instruction;
        IF_ID.nextPC = PC + 1;
        IF_ID.valid = true;
        incrementPC();

        printf("[IF] Fetched Instruction: 0x%04X | Next PC: 0x%04X\n", instruction, IF_ID.nextPC);
    }
}

/**
 * Instruction Decode (ID) Stage
 */
void decodeStage() {
    if (!IF_ID.valid) return;

    uint16_t instr = IF_ID.instruction;
    ID_EX.opcode = (instr >> 12) & 0x0F;
    ID_EX.r1 = (instr >> 6) & 0x3F;

    // R-Format or I-Format
    if (ID_EX.opcode == 0 || ID_EX.opcode == 1 || ID_EX.opcode == 2 || 
        ID_EX.opcode == 6 || ID_EX.opcode == 7) {
        // R-Format
        ID_EX.r2 = instr & 0x3F;
        ID_EX.isImmediate = false;
    } else {
        // I-Format
        uint8_t imm = instr & 0x3F;
        
        // Special handling for STR and LDR - addresses should be unsigned
        if (ID_EX.opcode == 10 || ID_EX.opcode == 11) {  // LDR or STR
            ID_EX.r2 = imm;  // Keep address as unsigned
        } else {
            // For other immediates, handle signed values
            if (imm & 0x20) {  // If bit 5 is set (negative number)
                ID_EX.r2 = imm | 0xC0;  // Sign extend to 8 bits
            } else {
                ID_EX.r2 = imm;  // Positive number, no sign extension needed
            }
        }
        ID_EX.isImmediate = true;
    }

    ID_EX.nextPC = IF_ID.nextPC;
    ID_EX.valid = true;

    // Print the decoded value appropriately based on instruction type
    if (ID_EX.opcode == 10 || ID_EX.opcode == 11) {
        printf("[ID] Decoded - Opcode: %d, R1: %d, Address: %d (0x%02X), Immediate? %d\n",
               ID_EX.opcode, ID_EX.r1, ID_EX.r2, ID_EX.r2, ID_EX.isImmediate);
    } else {
        printf("[ID] Decoded - Opcode: %d, R1: %d, R2/IMM: %d (0x%02X), Immediate? %d\n",
               ID_EX.opcode, ID_EX.r1, (int8_t)ID_EX.r2, ID_EX.r2, ID_EX.isImmediate);
    }
}

void executeStage() {
    if (!ID_EX.valid) return;

    printf("[EX] Executing Instruction - Opcode: %d\n", ID_EX.opcode);

    if (ID_EX.isImmediate) {
        // I-Format instructions
        switch (ID_EX.opcode) {
            case 3:  execute_MOVI(ID_EX.r1, ID_EX.r2); break;
            case 4:  if(IF_ID.valid){
                        setPC(PC-2);
                     }else{
                        setPC(PC-1);
                     }
                     execute_BEQZ(ID_EX.r1, ID_EX.r2);break;
            case 5:  execute_ANDI(ID_EX.r1, ID_EX.r2); break;
            case 8:  execute_SAL(ID_EX.r1, ID_EX.r2); break;
            case 9:  execute_SAR(ID_EX.r1, ID_EX.r2); break;
            case 10: execute_LDR(ID_EX.r1, ID_EX.r2); break;
            case 11: execute_STR(ID_EX.r1, ID_EX.r2); break;
            default:
                printf("[EX] Unknown I-Format Opcode: %d\n", ID_EX.opcode);
                break;
        }
    } else {
        // R-Format instructions
        switch (ID_EX.opcode) {
            case 0: execute_ADD(ID_EX.r1, ID_EX.r2); break;
            case 1: execute_SUB(ID_EX.r1, ID_EX.r2); break;
            case 2: execute_MUL(ID_EX.r1, ID_EX.r2); break;
            case 6: execute_EOR(ID_EX.r1, ID_EX.r2); break;
            case 7: execute_BR(ID_EX.r1, ID_EX.r2); break;
            default:
                printf("[EX] Unknown R-Format Opcode: %d\n", ID_EX.opcode);
                break;
        }
    }

    ID_EX.valid = false;
}

/**
 * Advances the pipeline by one cycle and prints the pipeline state.
 * Returns true if the pipeline is still active, false if it's fully drained.
 */
bool pipelineCycle() {
    printf("\n=========== Cycle %d ===========\n", ++cycle);
    executeStage();
    decodeStage();
    if(!isStalled){
        fetchStage();
    }
    isStalled = false;
    printPipelineState();
    printf("-------------------------------------\n");

    // Check if the pipeline is empty and halted
    if (isHalted && !IF_ID.valid && !ID_EX.valid) {
        return false; // Pipeline is drained
    }
    return true; // Continue execution
}

/**
 * Prints the current state of the pipeline.
 */
 void printPipelineState() {
    printf("==== Pipeline State ====\n");
    printf("IF/ID -> Instruction: 0x%04X | Next PC: 0x%04X | Valid: %d\n", 
           IF_ID.instruction, IF_ID.nextPC, IF_ID.valid);
    printf("ID/EX -> Opcode: %d | R1: %d | R2/Imm: %d | Format: %s | Next PC: 0x%04X | Valid: %d\n",
           ID_EX.opcode, ID_EX.r1, ID_EX.r2,
           ID_EX.isImmediate ? "I-Format" : "R-Format",
           ID_EX.nextPC, ID_EX.valid);
    printf("========================\n\n");
}