#ifndef PIPELINE_H
#define PIPELINE_H
#include <stdint.h>

typedef struct { uint16_t ir; uint16_t pc_if; } IF_ID;
typedef struct { uint16_t ir; uint8_t rd, rs1, rs2; int16_t imm; uint16_t pc_if; } ID_EX;
typedef struct { uint16_t ir; uint8_t result, rd; int memory_op; uint16_t mem_addr; } EX_WB;

void fetch_stage(IF_ID *if_id, int *fetch_allowed);
void decode_stage(const IF_ID *if_id, ID_EX *id_ex);
void execute_stage(const ID_EX *id_ex, EX_WB *ex_wb, int *flush, int *fetch_allowed);
void writeback_stage(const EX_WB *ex_wb);

#endif 