#ifndef CPUPARAMETERS_H
#define CPUPARAMETERS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "decode.h"

#define REG_COUNT 32
#define MEM_SIZE 256
#define MAX_INSTR 100

typedef struct {
    uint32_t instr_words[MAX_INSTR];
    int instr_count;
} Program;

typedef struct {
    uint32_t pc;
    int32_t reg[REG_COUNT];
    int32_t mem[MEM_SIZE];
} CPUState;

typedef struct {
    double clock_rate_hz;
    int pipeline_depth;
    int single_cycle_ipc;
    int pipelined_ipc;
    int branch_penalty;
    int enable_forwarding;
    int enable_hazard_detection;
} CPUParams;

typedef struct {
    int instruction_count;
    int total_cycles;
    int stall_cycles;
    int flush_cycles;
    int data_hazards;
    int control_hazards;

    double cpi;
    double latency_sec;
    double throughput_ips;
    double speedup;
} PerformanceMetrics;

typedef struct {
    uint32_t raw;
    uint32_t opcode;
    uint32_t rs;
    uint32_t rt;
    uint32_t rd;
    uint32_t shamt;
    uint32_t funct;
    int16_t  imm;
    uint32_t address;
    int valid;
} DecodedInstr;

typedef struct {
    int valid;
    uint32_t pc;
    uint32_t instr_word;
} IF_ID_Reg;

typedef struct {
    int valid;
    uint32_t pc;
    DecodedInstr instr;
    int32_t rs_value;
    int32_t rt_value;
} ID_EX_Reg;

typedef struct {
    int valid;
    uint32_t pc;
    DecodedInstr instr;
    int32_t alu_result;
    int32_t rt_value;
    int branch_taken;
    uint32_t branch_target;
} EX_MEM_Reg;

typedef struct {
    int valid;
    uint32_t pc;
    DecodedInstr instr;
    int32_t mem_data;
    int32_t alu_result;
} MEM_WB_Reg;

typedef struct {
    IF_ID_Reg if_id;
    ID_EX_Reg id_ex;
    EX_MEM_Reg ex_mem;
    MEM_WB_Reg mem_wb;
} PipelineState;

typedef struct {
    CPUState cpu;
    CPUParams params;
    PerformanceMetrics metrics;
    PipelineState pipe;
} Simulator;

void init_cpu_state(CPUState *cpu);
void init_cpu_params(CPUParams *params);
void init_metrics(PerformanceMetrics *m);
void init_pipeline_state(PipelineState *p);
void init_simulator(Simulator *sim);

DecodedInstr decode_word(uint32_t word);
void print_cpu_params(const CPUParams *params);
void print_pipeline_state(const PipelineState *p);
void print_metrics(const PerformanceMetrics *m);

#endif