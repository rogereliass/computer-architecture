#include "../includes/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Operation to opcode mapping
typedef struct {
    const char* op;
    uint8_t opcode;
} OpcodeMap;

static const OpcodeMap OPCODE_TABLE[] = {
    {"ADD",  0},
    {"SUB",  1},
    {"MUL",  2},
    {"MOVI", 3},
    {"BEQZ", 4},
    {"ANDI", 5},
    {"EOR",  6},
    {"BR",   7},
    {"SAL",  8},  // Shift instruction
    {"SAR",  9},  // Shift instruction
    {"LDR",  10},
    {"STR",  11}
};

#define OPCODE_TABLE_SIZE (sizeof(OPCODE_TABLE) / sizeof(OpcodeMap))

// Helper function to get opcode and instruction type
static uint8_t getOpcode(const char* op) {
    for (size_t i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (strcmp(op, OPCODE_TABLE[i].op) == 0) {
            return OPCODE_TABLE[i].opcode;
        }
    }
    return 0xFF; // Invalid opcode
}

// Helper function to parse register number (e.g., "R5" -> 5)
static uint8_t parseRegister(const char* reg) {
    if (reg[0] == 'R') {
        int reg_num = atoi(reg + 1);
        if (reg_num >= 0 && reg_num < 64) {  // Check register bounds
            return (uint8_t)reg_num;
        }
    }
    return 0xFF; // Invalid register
}

// Helper function to parse immediate value
static int8_t parseImmediate(const char* imm, int is_shift) {
    int value = atoi(imm);
    
    if (is_shift == 1) {
        // For shift instructions, ensure positive value and within bounds
        if (value < 0 || value > 7) {  // Shift amount should be 0-7
            printf("Error: Shift amount must be between 0 and 7\n");
            return 0xFF;  // Invalid shift amount
        }
        return (uint8_t)value;
    } else if (is_shift == 0) {
        // For other instructions, allow signed values (-32 to 31)
        if (value < -32 || value > 31) {
            printf("Error: Immediate value must be between -32 and 31\n");
            return 0xFF;  // Invalid immediate
        }
        return (int8_t)value;
    } else{
        if (value < 0 || value > 63) {
            printf("Error: Immediate value must be between 0 and 63\n");
            return 0xFF;  // Invalid immediate
        }
        return (uint8_t)value;
    }
}

// Helper function to create 16-bit instruction
static uint16_t createInstruction(uint8_t opcode, uint8_t operand1, int8_t operand2) {
    // Format: 4 bits opcode, 6 bits operand1, 6 bits operand2
    // For operand2, we need to handle signed values properly
    uint8_t operand2_unsigned;
    
    if (operand2 < 0) {
        // Convert negative value to 2's complement
        operand2_unsigned = (uint8_t)((int8_t)operand2 & 0x3F);  // Keep only 6 bits
    } else {
        operand2_unsigned = (uint8_t)operand2;
    }
    
    return ((uint16_t)opcode << 12) | ((uint16_t)operand1 << 6) | operand2_unsigned;
}

// Helper function to parse a single instruction line
static uint16_t parseInstructionLine(const char* line) {
    char op[10];
    char op1[10];
    char op2[10];
    
    // Skip empty lines and comments
    if (line[0] == '\n' || line[0] == '#' || line[0] == '\0') {
        return 0;
    }
    
    // Parse the instruction components
    if (sscanf(line, "%s %s %s", op, op1, op2) != 3) {
        printf("Error: Invalid instruction format: %s\n", line);
        return 0;
    }
    
    // Get opcode and check if it's a shift instruction
    //int is_shift = 0;
    uint8_t opcode = getOpcode(op);
    if (opcode == 0xFF) {
        printf("Error: Invalid operation: %s\n", op);
        return 0;
    }
    
    // Parse operands based on instruction type
    uint8_t operand1;
    int8_t operand2;
    
    // Handle different instruction formats
    if (strcmp(op, "MOVI") == 0 || strcmp(op, "BEQZ") == 0 || 
        strcmp(op, "ANDI") == 0) {
        // Format: OP R1, IMM
        operand1 = parseRegister(op1);
        operand2 = parseImmediate(op2, 0);
    } else if(strcmp(op, "SAL") == 0 || strcmp(op, "SAR") == 0) {
        operand1 = parseRegister(op1);
        operand2 = parseImmediate(op2, 1);
    } else if (strcmp(op, "LDR") == 0 || strcmp(op, "STR") == 0) {
        // Format: OP R1, ADDR
        operand1 = parseRegister(op1);
        operand2 = parseImmediate(op2, 2); 
    } else if (strcmp(op, "BR") == 0) {
        // Format: BR R1, R2
        operand1 = parseRegister(op1);
        operand2 = parseRegister(op2);
    } else {
        // Format: OP R1, R2 (ADD, SUB, MUL, EOR)
        operand1 = parseRegister(op1);
        operand2 = parseRegister(op2);
    }
    
    // Validate operands - use uint8_t for comparison since that's what parseRegister returns
    uint8_t operand1_check = parseRegister(op1);
    int8_t operand2_check = operand2;
    
    if (operand1_check == 0xFF || (uint8_t)operand2_check == 0xFF) {
        printf("Error: Invalid operands in instruction: %s\n", line);
        return 0;
    }
    
    // Create and return the 16-bit instruction
    return createInstruction(opcode, operand1, operand2);
}

/**
 * Parses a text file containing instructions and stores them in instruction memory.
 * @param filename: Path to the input text file
 * @return: Number of instructions successfully parsed and stored
 */
int parseInstructionFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }
    
    char line[256];
    int instructionCount = 0;
    uint16_t address = 0;
    
    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;
        
        // Skip empty lines and comments
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\0') {
            continue;
        }
        
        // Parse the instruction
        uint16_t instruction = parseInstructionLine(line);
        if (instruction != 0) {
            // Store in instruction memory
            writeToMemory(address++, instruction, 0);
            instructionCount++;
            
            // Print the parsed instruction for debugging
            printf("[PARSER] %s -> %d (0x%04X)\n", line, instruction, (uint16_t)instruction);
        }
    }
    
    // Add halt instruction (0xFFFF) at the end
    writeToMemory(address, 0xFFFF, 0);
    printf("[PARSER] HALT -> 0xFFFF\n");
    
    fclose(file);
    return instructionCount;
}

/**
 * Prints the binary representation of an instruction
 * @param instruction: 16-bit instruction to print
 */
void printInstructionBinary(uint16_t instruction) {
    printf("Binary: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (instruction >> i) & 1);
        if (i == 12 || i == 6) printf(" "); // Add spaces between fields
    }
    printf("\n");
}

