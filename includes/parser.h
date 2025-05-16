#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "memory.h"

/**
 * Parses a text file containing instructions and stores them in instruction memory.
 * @param filename: Path to the input text file
 * @return: Number of instructions successfully parsed and stored, or -1 on error
 */
int parseInstructionFile(const char* filename);

/**
 * Prints the binary representation of an instruction
 * @param instruction: 16-bit instruction to print
 */
void printInstructionBinary(uint16_t instruction);

#endif 