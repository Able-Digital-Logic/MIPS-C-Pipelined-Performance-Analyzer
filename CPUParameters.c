//Tien Hoang


#include "CPUParameters.h"

void init_cpu_state(CPUState *cpu) {
    int i;
    cpu->pc = 0;

    for (i = 0; i < REG_COUNT; i++) {
        cpu->reg[i] = 0;
    }

    for (i = 0; i < MEM_SIZE; i++) {
        cpu->mem[i] = 0;
    }
}

void init_cpu_params(CPUParams *params) {
    params->clock_rate_hz = 1e9;
    params->pipeline_depth = 5;
    params->single_cycle_ipc = 1;
    params->pipelined_ipc = 1;
}

void init_metrics(PerformanceMetrics *m) {
    m->instruction_count = 0;
    m->total_cycles = 0;
    m->stall_cycles = 0;
    m->flush_cycles = 0;
    m->data_hazards = 0;
    m->control_hazards = 0;

    m->cpi = 0.0;
    m->latency_sec = 0.0;
    m->throughput_ips = 0.0;
    m->speedup = 0.0;
}

void init_pipeline_state(PipelineState *p) {
    memset(p, 0, sizeof(PipelineState));
}

void init_simulator(Simulator *sim) {
    init_cpu_state(&sim->cpu);
    init_cpu_params(&sim->params);
    init_metrics(&sim->metrics);
    init_pipeline_state(&sim->pipe);
}

const char *opcode_name(OpCode op) {
    switch (op) {
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_ADDI: return "ADDI";
        case OP_LW: return "LW";
        case OP_SW: return "SW";
        case OP_BEQ: return "BEQ";
        case OP_NOP: return "NOP";
        default: return "INVALID";
    }
}

void print_cpu_params(const CPUParams *params) {
    printf("=== CPU Parameters ===\n");
    printf("Clock rate       : %.2f Hz\n", params->clock_rate_hz);
    printf("Pipeline depth   : %d\n", params->pipeline_depth);
    printf("Single-cycle IPC : %d\n", params->single_cycle_ipc);
    printf("Pipelined IPC    : %d\n", params->pipelined_ipc);
    printf("\n");
}

void print_pipeline_state(const PipelineState *p) {
    printf("=== Pipeline Registers ===\n");

    printf("IF/ID   : valid=%d, instr=%s\n",
           p->if_id.valid,
           opcode_name(p->if_id.instr.op));

    printf("ID/EX   : valid=%d, instr=%s\n",
           p->id_ex.valid,
           opcode_name(p->id_ex.instr.op));

    printf("EX/MEM  : valid=%d, instr=%s\n",
           p->ex_mem.valid,
           opcode_name(p->ex_mem.instr.op));

    printf("MEM/WB  : valid=%d, instr=%s\n",
           p->mem_wb.valid,
           opcode_name(p->mem_wb.instr.op));

    printf("\n");
}

void print_metrics(const PerformanceMetrics *m) {
    printf("=== Performance Metrics ===\n");
    printf("Instruction count : %d\n", m->instruction_count);
    printf("Total cycles      : %d\n", m->total_cycles);
    printf("Stall cycles      : %d\n", m->stall_cycles);
    printf("Flush cycles      : %d\n", m->flush_cycles);
    printf("Data hazards      : %d\n", m->data_hazards);
    printf("Control hazards   : %d\n", m->control_hazards);
    printf("CPI               : %.4f\n", m->cpi);
    printf("Latency (sec)     : %.8f\n", m->latency_sec);
    printf("Throughput (IPS)  : %.2f\n", m->throughput_ips);
    printf("Speedup           : %.4f\n", m->speedup);
    printf("\n");
}
