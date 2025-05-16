#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>      // For uint8_t and uint16_t types
#include <stdbool.h>     // For boolean values
#include "registers.h"
#include "memory.h"
#include "instruction_set.h"
#include "control.h"

// ======================= Pipeline Register Structures =======================

// IF/ID Register (Holds values between Fetch and Decode)
typedef struct {
    uint16_t instruction;  // 16-bit instruction fetched
    uint16_t nextPC;       // Address of the next instruction
    bool valid;            // If this stage holds valid data
} IF_ID_Reg;

// ID/EX Register (Holds values between Decode and Execute)
typedef struct {
    uint8_t opcode;        // Operation code
    uint8_t r1;            // Register 1 (6 bits)
    uint8_t r2;            // Register 2 or Immediate (6 bits)
    bool isImmediate;      // Whether this is I-Format or R-Format
    uint16_t nextPC;       // Next program counter value
    bool valid;            // If this stage holds valid data
} ID_EX_Reg;

// ======================= Pipeline Register Declarations =======================
extern IF_ID_Reg IF_ID;
extern ID_EX_Reg ID_EX;
extern bool isStalled;

// ======================= Pipeline Function Prototypes =======================
void initPipeline();
void fetchStage();
void decodeStage();
void executeStage();
bool pipelineCycle();
void printPipelineState();

#endif // PIPELINE_H