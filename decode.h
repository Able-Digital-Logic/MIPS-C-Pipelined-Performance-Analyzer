#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

#define MAX_INSTRUCTIONS  1024
#define MAX_LINE_LEN       256

//error code
#define UNKNOWN_INSTRUCTION  0xFFFFFFFFu


//register instruction
uint32_t encode_R(uint32_t funct, uint32_t rs, uint32_t rt, uint32_t rd,    uint32_t shamt);
//Immediate instruction
uint32_t encode_I(uint32_t op, uint32_t rs, uint32_t rt, int16_t imm);
//Jump instruction
uint32_t encode_J(uint32_t op, uint32_t target);
//register symbols to numbers
int reg_num(const char *name);
//instruction assembler
uint32_t assemble_instruction(const char *mnemonic, char *operands);

#endif
