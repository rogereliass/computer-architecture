#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "pipeline.h"
#include "registers.h"


// Function Prototypes
void flushPipeline();
void handleBranchFlush(uint16_t targetPC);
void stallPipeline();

#endif // CONTROL_H
