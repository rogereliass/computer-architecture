#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>

extern uint16_t instr_mem[1024];
extern uint8_t  data_mem[2048];

uint16_t load_instr(uint16_t addr);
uint8_t  load_data(uint16_t addr);
void     store_data(uint16_t addr, uint8_t val);

#endif 