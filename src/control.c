#include <stdio.h>
#include "../includes/control.h"


// ================== Pipeline Control Mechanisms ==================

/**
 * Flushes the pipeline stages to prevent incorrect execution.
 */
void flushPipeline() {
    IF_ID.valid = false;
    ID_EX.valid = false;
    isStalled = true;
    printf("[CONTROL] Pipeline flushed\n");
}

/**
 * Handles flushing and redirecting the pipeline for Branches (BEQZ).
 * @param targetPC: The target address to branch to.
 */
void handleBranchFlush(uint16_t targetPC) {
    flushPipeline();
    setPC(targetPC);
    printf("[CONTROL] Branch Taken -> Redirecting to 0x%04X\n", targetPC);
}

/**
 * Stalls the pipeline by invalidating IF/ID for one cycle.
 */
void stallPipeline() {
    IF_ID.valid = false;
    printf("[CONTROL] Pipeline Stalled for One Cycle\n");
}
