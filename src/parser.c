#include "parser.h"
#include "memory.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256
#define MAX_LABELS 100

typedef struct {
    char name[64];
    uint16_t address;
} Label;

static Label symbol_table[MAX_LABELS];
static int label_count = 0;

static char *trim(char *str) {
    while (isspace(*str)) str++;
    
    if (*str == 0) 
        return str;
        
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    
    end[1] = '\0';
    return str;
}

static int is_label(char *line) {
    char *colon = strchr(line, ':');
    return colon != NULL;
}

static void extract_label(char *line, char *label) {
    char *colon = strchr(line, ':');
    if (colon) {
        int length = colon - line;
        strncpy(label, line, length);
        label[length] = '\0';
        label = trim(label);
    }
}

static int get_register_number(const char *reg_str) {
    if (reg_str[0] == 'R' || reg_str[0] == 'r') {
        return atoi(reg_str + 1);
    }
    return -1;
}

static int16_t get_immediate(const char *imm_str) {
    return (int16_t)atoi(imm_str);
}

static int find_label_address(const char *label) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(symbol_table[i].name, label) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1;
}

// First pass: collect all labels and their addresses
static void collect_labels(FILE *fp) {
    char line[MAX_LINE_LENGTH];
    uint16_t addr = 0;
    
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        char *l = trim(line);
        
        // Skip empty lines and comments
        if (l[0] == '\0' || l[0] == ';' || l[0] == '#')
            continue;
            
        if (is_label(l)) {
            char label[64] = {0};
            extract_label(l, label);
            
            // Store the label and its address
            strcpy(symbol_table[label_count].name, label);
            symbol_table[label_count].address = addr;
            label_count++;
            
            // Check if there's an instruction after the label
            char *instr = strchr(l, ':');
            if (instr) {
                instr++; // Skip the colon
                instr = trim(instr);
                
                // If there's an instruction after the label, increment addr
                if (instr[0] != '\0' && instr[0] != ';' && instr[0] != '#')
                    addr++;
            }
        } else {
            // This line contains an instruction, increment address
            addr++;
        }
    }
    
    // Reset the file pointer to the beginning for the second pass
    rewind(fp);
}

// Second pass: assemble instructions
static void assemble_instructions(FILE *fp) {
    char line[MAX_LINE_LENGTH];
    uint16_t addr = 0;
    
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        char *l = trim(line);
        
        // Skip empty lines and comments
        if (l[0] == '\0' || l[0] == ';' || l[0] == '#')
            continue;
            
        char *instr_line = l;
        
        // If it's a label, get the instruction after it (if any)
        if (is_label(l)) {
            instr_line = strchr(l, ':');
            if (!instr_line || instr_line[1] == '\0' || instr_line[1] == ';' || instr_line[1] == '#')
                continue; // No instruction after label
                
            instr_line++; // Skip the colon
            instr_line = trim(instr_line);
        }
        
        // Parse the instruction
        char instr[16] = {0};
        char operands[MAX_LINE_LENGTH] = {0};
        
        // Split into instruction and operands
        char *space = strchr(instr_line, ' ');
        if (space) {
            strncpy(instr, instr_line, space - instr_line);
            instr[space - instr_line] = '\0';
            strcpy(operands, trim(space));
        } else {
            strcpy(instr, instr_line);
            operands[0] = '\0';
        }
        
        // Convert instruction to uppercase
        for (int i = 0; instr[i]; i++) {
            instr[i] = toupper(instr[i]);
        }
        
        uint16_t opcode = 0;
        
        // NOP instruction
        if (strcmp(instr, "NOP") == 0) {
            opcode = 0xFFFF; // NOP is 0xFFFF
        } 
        // R-type instructions
        else if (strcmp(instr, "ADD") == 0 || strcmp(instr, "SUB") == 0 || 
                 strcmp(instr, "MUL") == 0 || strcmp(instr, "EOR") == 0) {
            
            // Parse operands
            char *token;
            char *rd_str = NULL, *rs1_str = NULL, *rs2_str = NULL;
            char temp_operands[MAX_LINE_LENGTH];
            strcpy(temp_operands, operands);
            
            // Split operands by commas
            token = strtok(temp_operands, ",");
            if (token) rd_str = trim(token);
            
            token = strtok(NULL, ",");
            if (token) rs1_str = trim(token);
            
            token = strtok(NULL, ",");
            if (token) rs2_str = trim(token);
            
            int rd = rd_str ? get_register_number(rd_str) : 0;
            int rs1 = rs1_str ? get_register_number(rs1_str) : 0;
            int rs2 = rs2_str ? get_register_number(rs2_str) : 0;
            
            // Encode opcode
            if (strcmp(instr, "ADD") == 0) {
                opcode = 0;
            } else if (strcmp(instr, "SUB") == 0) {
                opcode = 1;
            } else if (strcmp(instr, "MUL") == 0) {
                opcode = 2;
            } else if (strcmp(instr, "EOR") == 0) {
                opcode = 4;
            }
            
            // Format for R-type: [4-bit opcode][6-bit rd][6-bit rs1][6-bit rs2]
            opcode = (opcode << 12) | ((rd & 0x3F) << 6) | (rs1 & 0x3F) | ((rs2 & 0x3F) << 0);
        }
        // I-type instructions
        else if (strcmp(instr, "ANDI") == 0 || strcmp(instr, "SAL") == 0 || 
                 strcmp(instr, "SAR") == 0 || strcmp(instr, "LOAD") == 0 || 
                 strcmp(instr, "STORE") == 0 || strcmp(instr, "BEQZ") == 0) {
            
            // Parse operands
            char *token;
            char *rd_str = NULL, *rs1_str = NULL, *rs2_str = NULL, *imm_str = NULL;
            char temp_operands[MAX_LINE_LENGTH];
            strcpy(temp_operands, operands);
            
            // Split operands by commas
            token = strtok(temp_operands, ",");
            
            if (strcmp(instr, "BEQZ") == 0) {
                rs1_str = trim(token);
                
                token = strtok(NULL, ",");
                if (token) imm_str = trim(token);
                
                // Check if immediate is a label
                if (imm_str && isalpha(imm_str[0])) {
                    int label_addr = find_label_address(imm_str);
                    if (label_addr != -1) {
                        // Calculate relative address
                        int16_t offset = label_addr - (addr + 1);
                        sprintf(imm_str, "%d", offset);
                    }
                }
            } else if (strcmp(instr, "STORE") == 0) {
                rs1_str = trim(token);
                
                token = strtok(NULL, ",");
                if (token) rs2_str = trim(token);
                
                token = strtok(NULL, ",");
                if (token) imm_str = trim(token);
            } else {
                rd_str = trim(token);
                
                token = strtok(NULL, ",");
                if (token) rs1_str = trim(token);
                
                token = strtok(NULL, ",");
                if (token) imm_str = trim(token);
            }
            
            int rd = rd_str ? get_register_number(rd_str) : 0;
            int rs1 = rs1_str ? get_register_number(rs1_str) : 0;
            int rs2 = rs2_str ? get_register_number(rs2_str) : 0;
            int16_t imm = imm_str ? get_immediate(imm_str) : 0;
            
            // Encode opcode
            if (strcmp(instr, "ANDI") == 0) {
                opcode = 3;
            } else if (strcmp(instr, "SAL") == 0) {
                opcode = 5;
            } else if (strcmp(instr, "SAR") == 0) {
                opcode = 6;
            } else if (strcmp(instr, "LOAD") == 0) {
                opcode = 7;
            } else if (strcmp(instr, "STORE") == 0) {
                opcode = 8;
            } else if (strcmp(instr, "BEQZ") == 0) {
                opcode = 9;
            }
            
            // Format for I-type: [4-bit opcode][6-bit rd/rs1][6-bit rs1/rs2][8-bit imm]
            if (strcmp(instr, "STORE") == 0) {
                opcode = (opcode << 12) | ((rs1 & 0x3F) << 6) | (rs2 & 0x3F) | (imm & 0xFF);
            } else if (strcmp(instr, "BEQZ") == 0) {
                opcode = (opcode << 12) | ((rs1 & 0x3F) << 6) | (imm & 0xFF);
            } else {
                opcode = (opcode << 12) | ((rd & 0x3F) << 6) | (rs1 & 0x3F) | (imm & 0xFF);
            }
        }
        // JR instruction
        else if (strcmp(instr, "JR") == 0) {
            // Parse operands
            char *token;
            char *r0_str = NULL, *r1_str = NULL;
            char temp_operands[MAX_LINE_LENGTH];
            strcpy(temp_operands, operands);
            
            // Split operands by commas
            token = strtok(temp_operands, ",");
            if (token) r0_str = trim(token);
            
            token = strtok(NULL, ",");
            if (token) r1_str = trim(token);
            
            int r0 = r0_str ? get_register_number(r0_str) : 0;
            int r1 = r1_str ? get_register_number(r1_str) : 0;
            
            // Encode JR (opcode 10)
            opcode = (10 << 12) | ((r0 & 0x3F) << 6) | (r1 & 0x3F);
        }
        
        // Store the assembled instruction
        instr_mem[addr++] = opcode;
    }
    
    // Set global PC to 0
    PC = 0;
}

void load_program(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    
    // Reset label count
    label_count = 0;
    
    // First pass: collect labels
    collect_labels(fp);
    
    // Second pass: assemble instructions
    assemble_instructions(fp);
    
    fclose(fp);
} 