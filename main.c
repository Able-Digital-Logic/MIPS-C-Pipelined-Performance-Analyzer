 /*
main file to structure all of the oher files around
ECE 5367 Final Project: Pipelined Performance Analyzer
Spring 2026
Jonathan McChesney-Fleming
most of this information is sourced from:
https://student.cs.uwaterloo.ca/~isg/res/mips/opcodes 


*/
//included a bunch idk if they are all needed
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "decode.h"

#define Memsize 1024 //memory limit for resources
#define MAX_INSTRUCTIONS 1024
#define MAX_LINE_LEN 256
//#define UNKNOWN_INSTRUCTION 0xFFFFFFFF //error code

//Do i need to do all of this?
//instruction fetch
struct IFstruct {
	
	
};
//Instrucion decode
struct IDstruct {
	
	
};
//Execute
struct EXstruct {
	
};
// Memory
struct MEMstruct {
	
};
//Writeback
struct WBstruct{
	
};


//Types of instructions //not sure if we need
typedef enum{
	RegisterType,
	ImmediateType,
	JumpType,
}instructiontypes;


uint32_t assemble_instruction(const char *mnemonic, char *operands);

//wrote outline for main, fed claude the outline, and the decode files, along with the outline, spliced it back
int main(int argc, char **argv)
{
	
	
	
//main section

    FILE     *fp;                        //source file handle
    uint32_t  memory[MAX_INSTRUCTIONS];  //assembled instructions words
    int       count;                     //number of instructions assembled
    char      line[MAX_LINE_LEN];        // one line read from the source file
    char     *trimmed;                   /* pointer past leading whitespace    */
    char     *comment;                   /* pointer to first '#' on a line     */
    char      mnemonic[32];              /* instruction name token             */
    char      operands[MAX_LINE_LEN];    /* everything after the mnemonic      */
    uint32_t  word;                      //encoded 32-bit instruction word

//check for source file, in the same file path
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <TestFile.txt>\n", argv[0]); //replace with your test file name
        return 1;
    }
//open source file
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }
    count = 0; //set count to 0



//read the input file, translate it into instruction sets
	while (fgets(line, sizeof(line), fp) != NULL && count < MAX_INSTRUCTIONS) {		//read the file
		line[ strcspn(line, "\n") ] = '\0'; //remove newline char
		
		trimmed = line; //trimm the white space if present
        while (*trimmed == ' ' || *trimmed == '\t') {
            trimmed++;
        }
        
        if (*trimmed == '\0' || *trimmed == '#') {//skip blank lines and comments
            continue;
        }
        comment = strchr(trimmed, '#');
        if (comment != NULL) {
            *comment = '\0';
        }
        //Split the line into a mnemonic and an operand string.
        mnemonic[0] = '\0';
        operands[0] = '\0';
        sscanf(trimmed, "%31s %[^\n]", mnemonic, operands);
		
		//reference the instruction type it is, encoding the instruction
		 word = assemble_instruction(mnemonic, operands);

        if (word == UNKNOWN_INSTRUCTION) {
            fprintf(stderr,
                    "Warning: unrecognised instruction '%s' -- line skipped\n",
                    mnemonic);
            continue;
        }
		//append to memory as binary, and report
		memory[count] = word;

        printf("[%3d]  %-8s %-30s  =>  0x%08X\n",
               count, mnemonic, operands, word);

        count++;
	}
	//close the file
	fclose(fp);
    printf("\n%d instruction(s) assembled into memory.\n", count);
    
//run through the instructions with given input
//each instruction needs to look 5 ahead i think since there are 5 steps for each


	//WB reads from current state wb
	//MEM writes to newstate wb
	//ex writes into newstate.mem
	//ID writes into newstate ex
	//if writes into newstate id
	//state = newstate (next clock cylce)
	
	return 0;
}

