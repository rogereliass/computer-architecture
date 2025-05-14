#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>

extern uint8_t  GPR[64];
extern uint16_t PC;
extern uint8_t  SREG;

#define FLAG_C 0x01
#define FLAG_V 0x02
#define FLAG_N 0x04
#define FLAG_S 0x08
#define FLAG_Z 0x10
#define NOP    0xFFFF

#endif 