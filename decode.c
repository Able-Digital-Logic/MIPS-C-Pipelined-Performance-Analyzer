/*
 * MIPS encoding reference from:
 * https://student.cs.uwaterloo.ca/~isg/res/mips/opcodes
 *   R-format:  ooooooss sssttttt dddddaaa aaffffff   (32 bits)
 *   I-format:  ooooooss sssttttt iiiiiiii iiiiiiii   (32 bits)
 *   J-format:  ooooooii iiiiiiii iiiiiiii iiiiiiii   (32 bits)
*/

#include "decode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rtype_entry {
    const char *name;
    uint32_t funct;
};

static struct rtype_entry rtype_table[] = {
    { "add",  0x20 },
    { "addu", 0x21 },
    { "and",  0x24 },
    { "nor",  0x27 },
    { "or",   0x25 },
    { "sub",  0x22 },
    { "subu", 0x23 },
    { "xor",  0x26 },
    { "slt",  0x2A },
    { "sltu", 0x2B },
    { NULL,   0    }
};

//Commented out to decrease complexity
/*
//R-type DivMult instructions: syntax  f $s, $t
static struct rtype_entry divmult_table[] = {
    { "div",   0x1A },
    { "divu",  0x1B },
    { "mult",  0x18 },
    { "multu", 0x19 },
    { NULL,    0    }
};


//R-type Shift instructions: syntax  f $d, $t, a
static struct rtype_entry shift_table[] = {
    { "sll", 0x00 },
    { "sra", 0x03 },
    { "srl", 0x02 },
    { NULL,  0    }
};

//R-type ShiftV instructions: syntax  f $d, $t, $s
static struct rtype_entry shiftv_table[] = {
    { "sllv", 0x04 },
    { "srav", 0x07 },
    { "srlv", 0x06 },
    { NULL,   0    }
};
*/

//I-type ArithLogI instructions: syntax  o $t, $s, i
struct itype_entry {
    const char *name;
    uint32_t op;
};

static struct itype_entry itype_table[] = {
    { "addi",  0x08 },
    { "addiu", 0x09 },
    { "andi",  0x0C },
    { "ori",   0x0D },
    { "xori",  0x0E },
    { "slti",  0x0A },
    { "sltiu", 0x0B },
    { NULL,    0    }
};

static struct itype_entry loadstore_table[] = {
    { "lb",  0x20 },
    { "lbu", 0x24 },
    { "lh",  0x21 },
    { "lhu", 0x25 },
    { "lw",  0x23 },
    { "sb",  0x28 },
    { "sh",  0x29 },
    { "sw",  0x2B },
    { NULL,  0    }
};

//I-type BranchZ instructions: syntax  o $s, offset
	//this done later since we need a second parse
/*
struct branchz_entry {
    const char *name;
    uint32_t    op;
    uint32_t    rt;    //encodes the condition, not a register 
};
*/

//static struct branchz_entry branchz_table[] = {
//    { "bgez", 0x01, 0x01 },
 //   { "bgtz", 0x07, 0x00 },
 //   { "blez", 0x06, 0x00 },
 //   { "bltz", 0x01, 0x00 },
 //   { NULL,   0,    0    }
//};

//======================================================================
//Encoding section
//======================================================================

// encode R
//  * Field positions (from the MIPS Encoding Reference):
//     *   opcode  bits 31-26  (always 000000 for R-type)
//     *   rs      bits 25-21
//     *   rt      bits 20-16
//     *   rd      bits 15-11
//     *   shamt   bits 10-6
//     *   funct   bits  5-0
uint32_t encode_R(uint32_t funct, uint32_t rs, uint32_t rt, uint32_t rd, uint32_t shamt) {
    uint32_t word;
    word  = (0u << 26);
    word |= (rs << 21);
    word |= (rt << 16);
    word |= (rd << 11);
    word |= (shamt << 6);
    word |= (funct & 0x3Fu);
    return word;
}

//encode I
//     * Field positions:
//     *   opcode     bits 31-26
//     *   rs         bits 25-21
//     *   rt         bits 20-16
//     *   immediate  bits 15-0
uint32_t encode_I(uint32_t op, uint32_t rs, uint32_t rt, int16_t imm) {
    uint32_t word;
    word  = (op << 26);
    word |= (rs << 21);
    word |= (rt << 16);
    word |= (uint16_t)imm;
    return word;
}

//encode J
// * Field positions:
//     *   opcode  bits 31-26
//     *   target  bits 25-0
uint32_t encode_J(uint32_t op, uint32_t target) {
    uint32_t word;
    word  = (op << 26);
    word |= (target & 0x03FFFFFFu);
    return word;
}

//======================================================================
//REgister section
//======================================================================
int reg_num(const char *name) {
    const char *r = (name[0] == '$') ? name + 1 : name;
		//plus 1 to skip over the $ for register names
    if (r[0] >= '0' && r[0] <= '9') {
        return atoi(r);
    }
	
		//ai used to scrape the https://student.cs.uwaterloo.ca/~isg/res/mips/opcodes website
		//and return this
    if (strcmp(r, "zero") == 0) return 0;
    if (strcmp(r, "at")   == 0) return 1;
    if (strcmp(r, "v0")   == 0) return 2;
    if (strcmp(r, "v1")   == 0) return 3;
    if (strcmp(r, "a0")   == 0) return 4;
    if (strcmp(r, "a1")   == 0) return 5;
    if (strcmp(r, "a2")   == 0) return 6;
    if (strcmp(r, "a3")   == 0) return 7;
    if (strcmp(r, "t0")   == 0) return 8;
    if (strcmp(r, "t1")   == 0) return 9;
    if (strcmp(r, "t2")   == 0) return 10;
    if (strcmp(r, "t3")   == 0) return 11;
    if (strcmp(r, "t4")   == 0) return 12;
    if (strcmp(r, "t5")   == 0) return 13;
    if (strcmp(r, "t6")   == 0) return 14;
    if (strcmp(r, "t7")   == 0) return 15;
    if (strcmp(r, "s0")   == 0) return 16;
    if (strcmp(r, "s1")   == 0) return 17;
    if (strcmp(r, "s2")   == 0) return 18;
    if (strcmp(r, "s3")   == 0) return 19;
    if (strcmp(r, "s4")   == 0) return 20;
    if (strcmp(r, "s5")   == 0) return 21;
    if (strcmp(r, "s6")   == 0) return 22;
    if (strcmp(r, "s7")   == 0) return 23;
    if (strcmp(r, "t8")   == 0) return 24;
    if (strcmp(r, "t9")   == 0) return 25;
    if (strcmp(r, "k0")   == 0) return 26;
    if (strcmp(r, "k1")   == 0) return 27;
    if (strcmp(r, "gp")   == 0) return 28;
    if (strcmp(r, "sp")   == 0) return 29;
    if (strcmp(r, "fp")   == 0) return 30;
    if (strcmp(r, "ra")   == 0) return 31;

    fprintf(stderr, "Warning: unknown register name '%s'\n", name);
    return -1;
}


//======================================================================
//Assemble section
//======================================================================

//used ai entirely for this function, will test for each one
uint32_t assemble_instruction(const char *mnemonic, char *operands) {
    char a[32], b[32], c[32], base[32];
    int16_t offset;
    int i;

    a[0] = b[0] = c[0] = base[0] = '\0';
    offset = 0;


		//R types===========================================================
		//R-type: ArithLog   syntax:  f $d, $s, $t
    for (i = 0; rtype_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, rtype_table[i].name) == 0) {
            sscanf(operands, "%[^,], %[^,], %s", a, b, c);
            return encode_R(rtype_table[i].funct,
                            reg_num(b), /* rs = second operand  */
                            reg_num(c), /* rt = third operand   */
                            reg_num(a), /* rd = first operand   */
                            0);/* shamt not used       */
        }
    }

    for (i = 0; itype_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, itype_table[i].name) == 0) {
            sscanf(operands, "%[^,], %[^,], %s", a, b, c);
            return encode_I(itype_table[i].op,
                            reg_num(b),
                            reg_num(a),
                            (int16_t)strtol(c, NULL, 0));
        }
    }

    for (i = 0; loadstore_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, loadstore_table[i].name) == 0) {
            sscanf(operands, "%[^,], %hd(%[^)])", a, &offset, base);
            return encode_I(loadstore_table[i].op,
                            reg_num(base),
                            reg_num(a),
                            offset);
        }
    }

    if (strcmp(mnemonic, "beq") == 0) {
        sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        return encode_I(0x04u,
                        reg_num(a),
                        reg_num(b),
                        (int16_t)strtol(c, NULL, 0));
    }

    if (strcmp(mnemonic, "j") == 0) {
        sscanf(operands, "%s", a);
        return encode_J(0x02u, (uint32_t)strtol(a, NULL, 0));
    }

    if (strcmp(mnemonic, "nop") == 0) {
        return 0x00000000u;
    }

    return UNKNOWN_INSTRUCTION;
}

int parse_instruction_fields(const char *mnemonic, char *operands, Instruction *instr) {
    char a[32], b[32], c[32];
    int n;

    a[0] = b[0] = c[0] = '\0';

    instr->op = OP_INVALID;
    instr->rs = 0;
    instr->rt = 0;
    instr->rd = 0;
    instr->imm = 0;
    instr->label[0] = '\0';

    if (strcmp(mnemonic, "add") == 0) {
        n = sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        if (n == 3) {
            instr->op = OP_ADD;
            instr->rd = reg_num(a);
            instr->rs = reg_num(b);
            instr->rt = reg_num(c);
            return 1;
        }
    }

    if (strcmp(mnemonic, "sub") == 0) {
        n = sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        if (n == 3) {
            instr->op = OP_SUB;
            instr->rd = reg_num(a);
            instr->rs = reg_num(b);
            instr->rt = reg_num(c);
            return 1;
        }
    }

    if (strcmp(mnemonic, "and") == 0) {
        n = sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        if (n == 3) {
            instr->op = OP_AND;
            instr->rd = reg_num(a);
            instr->rs = reg_num(b);
            instr->rt = reg_num(c);
            return 1;
        }
    }

    if (strcmp(mnemonic, "or") == 0) {
        n = sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        if (n == 3) {
            instr->op = OP_OR;
            instr->rd = reg_num(a);
            instr->rs = reg_num(b);
            instr->rt = reg_num(c);
            return 1;
        }
    }

    if (strcmp(mnemonic, "addi") == 0) {
        n = sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        if (n == 3) {
            instr->op = OP_ADDI;
            instr->rt = reg_num(a);
            instr->rs = reg_num(b);
            instr->imm = (int)strtol(c, NULL, 0);
            return 1;
        }
    }

    if (strcmp(mnemonic, "lw") == 0) {
        n = sscanf(operands, "%[^,], %d(%[^)])", a, &instr->imm, b);
        if (n == 3) {
            instr->op = OP_LW;
            instr->rt = reg_num(a);
            instr->rs = reg_num(b);
            return 1;
        }
    }

    if (strcmp(mnemonic, "sw") == 0) {
        n = sscanf(operands, "%[^,], %d(%[^)])", a, &instr->imm, b);
        if (n == 3) {
            instr->op = OP_SW;
            instr->rt = reg_num(a);
            instr->rs = reg_num(b);
            return 1;
        }
    }

    if (strcmp(mnemonic, "beq") == 0) {
        n = sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        if (n == 3) {
            instr->op = OP_BEQ;
            instr->rs = reg_num(a);
            instr->rt = reg_num(b);
            instr->imm = (int)strtol(c, NULL, 0);
            return 1;
        }
    }

    if (strcmp(mnemonic, "j") == 0) {
        n = sscanf(operands, "%s", a);
        if (n == 1) {
            instr->op = OP_J;
            instr->target = (int)strtol(a, NULL, 0);
            return 1;
        }
    }

    if (strcmp(mnemonic, "nop") == 0) {
        instr->op = OP_NOP;
        return 1;
    }

    return 0;
}

//added later since we need to dissasemble
int disassemble_word(uint32_t word, Instruction *instr) {
    uint32_t op, rs, rt, rd, shamt, funct, imm16;
    int16_t signed_imm;
    uint32_t target;

    /* Initialize instruction */
    instr->op = OP_INVALID;
    instr->rs = 0;
    instr->rt = 0;
    instr->rd = 0;
    instr->imm = 0;
    instr->target = 0;
    instr->label[0] = '\0';

    /* Extract fields */
    op = (word >> 26) & 0x3F;
    rs = (word >> 21) & 0x1F;
    rt = (word >> 16) & 0x1F;
    rd = (word >> 11) & 0x1F;
    shamt = (word >> 6) & 0x1F;
    funct = word & 0x3F;
    imm16 = word & 0xFFFF;
    signed_imm = (int16_t)imm16;
    target = word & 0x03FFFFFF;

    /* R-type instructions (op == 0) */
    if (op == 0x00) {
        if (word == 0x00000000) {
            instr->op = OP_NOP;
            return 1;
        }

        instr->rs = rs;
        instr->rt = rt;
        instr->rd = rd;

        switch (funct) {
            case 0x20: instr->op = OP_ADD; return 1;
            case 0x22: instr->op = OP_SUB; return 1;
            case 0x24: instr->op = OP_AND; return 1;
            case 0x25: instr->op = OP_OR;  return 1;
            default: return 0;  /* Unsupported R-type */
        }
    }

    /* J-type instructions */
    if (op == 0x02) {  /* j */
        instr->op = OP_J;
        instr->target = target;
        return 1;
    }

    /* I-type instructions */
    instr->rs = rs;
    instr->rt = rt;
    instr->imm = signed_imm;

    switch (op) {
        case 0x04: instr->op = OP_BEQ;  return 1;
        case 0x08: instr->op = OP_ADDI; return 1;
        case 0x23: instr->op = OP_LW;   return 1;
        case 0x2B: instr->op = OP_SW;   return 1;
        default: return 0;  /* Unsupported opcode */
    }
}

//done
