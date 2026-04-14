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

//R-type ArithLog instructions: syntax  f $d, $s, $t
struct rtype_entry {
    const char *name;
    uint32_t    funct;
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
    { NULL,   0    }   /* sentinel */
};

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

//I-type ArithLogI instructions: syntax  o $t, $s, i
struct itype_entry {
    const char *name;
    uint32_t    op;
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

//I-type LoadStore instructions: syntax  o $t, offset($s)
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
struct branchz_entry {
    const char *name;
    uint32_t    op;
    uint32_t    rt;   /* encodes the condition, not a register */
};

static struct branchz_entry branchz_table[] = {
    { "bgez", 0x01, 0x01 },
    { "bgtz", 0x07, 0x00 },
    { "blez", 0x06, 0x00 },
    { "bltz", 0x01, 0x00 },
    { NULL,   0,    0    }
};
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
uint32_t encode_R(uint32_t funct, uint32_t rs, uint32_t rt, uint32_t rd, uint32_t shamt){
	uint32_t word;
    word  = (0u    << 26);        /* opcode: 000000 for all R-type */
    word |= (rs    << 21);
    word |= (rt    << 16);
    word |= (rd    << 11);
    word |= (shamt <<  6);
    word |= (funct & 0x3Fu);      /* mask to 6 bits as a safeguard */

    return word;
};

//encode I
//     * Field positions:
//     *   opcode     bits 31-26
//     *   rs         bits 25-21
//     *   rt         bits 20-16
//     *   immediate  bits 15-0
uint32_t encode_I(uint32_t op, uint32_t rs, uint32_t rt, int16_t imm){
	uint32_t word;

    word  = (op << 26);
    word |= (rs << 21);
    word |= (rt << 16);
    word |= (uint16_t) imm;       /* cast strips sign-extension */

    return word;
};
//encode J
// * Field positions:
//     *   opcode  bits 31-26
//     *   target  bits 25-0
uint32_t encode_J(uint32_t op, uint32_t target){
	uint32_t word;

    word  = (op << 26);
    word |= (target & 0x03FFFFFFu);

    return word;
};

//======================================================================
//REgister section
//======================================================================
int reg_num(const char *name)
{
    const char *r;
		//plus 1 to skip over the $ for register names
    r = (name[0] == '$') ? name + 1 : name;
    //if name is $number, it already references the number, return asap
    if (r[0] >= '0' && r[0] <= '9') {
        return atoi(r);
    }
		//ai used to scrape the https://student.cs.uwaterloo.ca/~isg/res/mips/opcodes website
		//and return this
	if (strcmp(r, "zero") == 0) return  0;
    if (strcmp(r, "at")   == 0) return  1;
    if (strcmp(r, "v0")   == 0) return  2;
    if (strcmp(r, "v1")   == 0) return  3;
    if (strcmp(r, "a0")   == 0) return  4;
    if (strcmp(r, "a1")   == 0) return  5;
    if (strcmp(r, "a2")   == 0) return  6;
    if (strcmp(r, "a3")   == 0) return  7;
    if (strcmp(r, "t0")   == 0) return  8;
    if (strcmp(r, "t1")   == 0) return  9;
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
uint32_t assemble_instruction(const char *mnemonic, char *operands)
{
	char     a[32];
    char     b[32];
    char     c[32];
    char     base[32];    /* base register for load/store: offset($base) */
    int16_t  offset;      /* signed offset for load/store instructions   */
    uint32_t op;          /* opcode for branch instructions              */
    int      i;           /* loop index for table searches               */

    a[0] = b[0] = c[0] = base[0] = '\0';
    offset = 0;
    
    //R types===========================================================
    //R-type: ArithLog   syntax:  f $d, $s, $t
    for (i = 0; rtype_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, rtype_table[i].name) == 0) {
            sscanf(operands, "%[^,], %[^,], %s", a, b, c);
            return encode_R(rtype_table[i].funct,
                            reg_num(b),   /* rs = second operand  */
                            reg_num(c),   /* rt = third operand   */
                            reg_num(a),   /* rd = first operand   */
                            0);           /* shamt not used       */
        }
	}
    
	//R-type: DivMult    syntax:  f $s, $t
    for (i = 0; divmult_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, divmult_table[i].name) == 0) {
            sscanf(operands, "%[^,], %s", a, b);
            return encode_R(divmult_table[i].funct,
                            reg_num(a),   /* rs */
                            reg_num(b),   /* rt */
                            0,            /* rd not used  */
                            0);           /* shamt not used */
        }
    }
    //R-type: Shift      syntax:  f $d, $t, a
    for (i = 0; shift_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, shift_table[i].name) == 0) {
            sscanf(operands, "%[^,], %[^,], %s", a, b, c);
            return encode_R(shift_table[i].funct,
                            0,            /* rs = 0 for constant shift */
                            reg_num(b),   /* rt = source register      */
                            reg_num(a),   /* rd = destination register */
                            atoi(c));     /* shamt = constant amount   */
        }
    }
	//R-type: ShiftV     syntax:  f $d, $t, $s
    for (i = 0; shiftv_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, shiftv_table[i].name) == 0) {
            sscanf(operands, "%[^,], %[^,], %s", a, b, c);
            return encode_R(shiftv_table[i].funct,
                            reg_num(c),   /* rs = shift-amount register */
                            reg_num(b),   /* rt = register to shift     */
                            reg_num(a),   /* rd = destination           */
                            0);           /* shamt = 0 for variable shift */
        }
    }
	//R-type: JumpR      syntax:  jr $s  /  jalr $d, $s
	if (strcmp(mnemonic, "jr") == 0) {
        sscanf(operands, "%s", a);
        return encode_R(0x08,
                        reg_num(a),   /* rs = register containing address */
                        0, 0, 0);
    }

    if (strcmp(mnemonic, "jalr") == 0) {
        sscanf(operands, "%[^,], %s", a, b);
        return encode_R(0x09,
                        reg_num(b),   /* rs = address register  */
                        0,
                        reg_num(a),   /* rd = return-address destination */
                        0);
    }
    //R-type: MoveFrom / MoveTo  hi and lo registers
	if (strcmp(mnemonic, "mfhi") == 0) {
        sscanf(operands, "%s", a);
        return encode_R(0x10, 0, 0, reg_num(a), 0);
    }
    if (strcmp(mnemonic, "mflo") == 0) {
        sscanf(operands, "%s", a);
        return encode_R(0x12, 0, 0, reg_num(a), 0);
    }
    if (strcmp(mnemonic, "mthi") == 0) {
        sscanf(operands, "%s", a);
        return encode_R(0x11, reg_num(a), 0, 0, 0);
    }
    if (strcmp(mnemonic, "mtlo") == 0) {
        sscanf(operands, "%s", a);
        return encode_R(0x13, reg_num(a), 0, 0, 0);
    }
    // I-types==========================================================
    //I-type: ArithLogI  syntax:  o $t, $s, i
    for (i = 0; itype_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, itype_table[i].name) == 0) {
            sscanf(operands, "%[^,], %[^,], %s", a, b, c);
            return encode_I(itype_table[i].op,
                            reg_num(b),                      /* rs */
                            reg_num(a),                      /* rt */
                            (int16_t) strtol(c, NULL, 0));   /* imm */
        }
    }
    //I-type: LoadI      syntax:  o $t, immed32
    if (strcmp(mnemonic, "lhi") == 0) {
        sscanf(operands, "%[^,], %s", a, b);
        return encode_I(0x19, 0, reg_num(a),
                        (int16_t) strtol(b, NULL, 0));
    }
    if (strcmp(mnemonic, "llo") == 0) {
        sscanf(operands, "%[^,], %s", a, b);
        return encode_I(0x18, 0, reg_num(a),
                        (int16_t) strtol(b, NULL, 0));
    }
    //I-type: Branch     syntax:  o $s, $t, offset
    if (strcmp(mnemonic, "beq") == 0 || strcmp(mnemonic, "bne") == 0) {
        op = (strcmp(mnemonic, "beq") == 0) ? 0x04u : 0x05u;
        sscanf(operands, "%[^,], %[^,], %s", a, b, c);
        return encode_I(op,
                        reg_num(a),                      /* rs */
                        reg_num(b),                      /* rt */
                        (int16_t) strtol(c, NULL, 0));   /* offset */
    }
    //I-type: BranchZ    syntax:  o $s, offset
    for (i = 0; branchz_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, branchz_table[i].name) == 0) {
            sscanf(operands, "%[^,], %s", a, b);
            return encode_I(branchz_table[i].op,
                            reg_num(a),                      /* rs */
                            branchz_table[i].rt,             /* condition code, not a register */
                            (int16_t) strtol(b, NULL, 0));   /* offset */
        }
    }
    //I-type: LoadStore  syntax:  o $t, offset($s)
    for (i = 0; loadstore_table[i].name != NULL; i++) {
        if (strcmp(mnemonic, loadstore_table[i].name) == 0) {
            sscanf(operands, "%[^,], %hd(%[^)])", a, &offset, base);
            return encode_I(loadstore_table[i].op,
                            reg_num(base),   /* rs = base register */
                            reg_num(a),      /* rt = data register */
                            offset);
        }
    }
    //J-types===========================================================
    //J-type: Jump       syntax:  o label
    if (strcmp(mnemonic, "j") == 0) {
        sscanf(operands, "%s", a);
        return encode_J(0x02, (uint32_t) strtol(a, NULL, 0));
    }
    if (strcmp(mnemonic, "jal") == 0) {
        sscanf(operands, "%s", a);
        return encode_J(0x03, (uint32_t) strtol(a, NULL, 0));
    }
    //J-type: Trap       syntax:  o i
    if (strcmp(mnemonic, "trap") == 0) {
        sscanf(operands, "%s", a);
        return encode_J(0x1A, (uint32_t) strtol(a, NULL, 0));
    }
    //if none then return this
	return UNKNOWN_INSTRUCTION;
}
