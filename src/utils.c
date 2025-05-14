#include "utils.h"

// Register file and processor state
uint8_t GPR[64] = {0};  // General purpose registers (R0 is hardwired to 0)
uint16_t PC = 0;        // Program counter
uint8_t SREG = 0;       // Status register 