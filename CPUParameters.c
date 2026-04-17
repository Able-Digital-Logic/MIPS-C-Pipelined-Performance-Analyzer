#include "CPUParameters.h"
#include <string.h>

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
    params->branch_penalty = 2;
    params->enable_forwarding = 0;
    params->enable_hazard_detection = 1;
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

DecodedInstr decode_word(uint32_t word) {
    DecodedInstr d;

    d.raw = word;
    d.opcode = (word >> 26) & 0x3F;
    d.rs = (word >> 21) & 0x1F;
    d.rt = (word >> 16) & 0x1F;
    d.rd = (word >> 11) & 0x1F;
    d.shamt = (word >> 6) & 0x1F;
    d.funct = word & 0x3F;
    d.imm = (int16_t)(word & 0xFFFF);
    d.address = word & 0x03FFFFFFu;
    d.valid = (word != UNKNOWN_INSTRUCTION);

    return d;
}

void print_cpu_params(const CPUParams *params) {
    printf("=== CPU Parameters ===\n");
    printf("Clock rate          : %.2f Hz\n", params->clock_rate_hz);
    printf("Pipeline depth      : %d\n", params->pipeline_depth);
    printf("Single-cycle IPC    : %d\n", params->single_cycle_ipc);
    printf("Pipelined IPC       : %d\n", params->pipelined_ipc);
    printf("Branch penalty      : %d\n", params->branch_penalty);
    printf("Forwarding enabled  : %d\n", params->enable_forwarding);
    printf("Hazard detection    : %d\n", params->enable_hazard_detection);
    printf("\n");
}

void print_pipeline_state(const PipelineState *p) {
    printf("=== Pipeline Registers ===\n");

    printf("IF/ID   : valid=%d, word=0x%08X\n",
           p->if_id.valid,
           p->if_id.instr_word);

    printf("ID/EX   : valid=%d, opcode=0x%02X, funct=0x%02X\n",
           p->id_ex.valid,
           p->id_ex.instr.opcode,
           p->id_ex.instr.funct);

    printf("EX/MEM  : valid=%d, opcode=0x%02X, alu_result=%d\n",
           p->ex_mem.valid,
           p->ex_mem.instr.opcode,
           p->ex_mem.alu_result);

    printf("MEM/WB  : valid=%d, opcode=0x%02X, mem_data=%d, alu_result=%d\n",
           p->mem_wb.valid,
           p->mem_wb.instr.opcode,
           p->mem_wb.mem_data,
           p->mem_wb.alu_result);

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