# MIPS-C-Pipelined-Performance-Analyzer

A comprehensive MIPS instruction set simulator that models both single-cycle and pipelined CPU architectures. This tool analyzes program performance, detects hazards, and generates detailed performance metrics to compare architectural approaches.

## Overview

This simulator processes MIPS assembly code or machine code and executes it through two different CPU models:
- **Single-Cycle CPU**: Each instruction completes in one clock cycle
- **Pipelined CPU**: 5-stage pipeline (IF, ID, EX, MEM, WB) with hazard detection and handling

The simulator generates performance reports including:
- Instruction counts by type
- Cycle counts and CPI (Cycles Per Instruction)
- Hazard detection (data hazards, control hazards)
- Execution time calculations
- Speedup analysis (pipelined vs. single-cycle)
- Detailed cycle-by-cycle trace files

This project largely references the website:
	https://student.cs.uwaterloo.ca/~isg/res/mips/opcodes
	This website has listed all of the instructions, some of which are commented out for simplicity of the file, and the input files given by the professor.
## Project Structure

### Source Files

| File | Purpose |
|------|---------|
| **main.c** | Program entry point, file loading, orchestration |
| **simulate.c** | Core simulation engines (single-cycle and pipeline) |
| **simulate.h** | Simulation function declarations |
| **decode.c** | Instruction encoding/decoding, assembly parsing |
| **decode.h** | Decoder function declarations |
| **CPUParameters.c** | CPU state initialization and utilities |
| **CPUParameters.h** | Type definitions, structures, constants |

### Key Components

#### CPUParameters.h/c
- Defines all CPU structures (registers, memory, pipeline stages)
- Instruction types and opcodes
- Performance metrics tracking
- CPU parameter initialization

#### decode.h/c
- MIPS instruction encoding/decoding
- Assembly language parsing with label support
- Machine code disassembly
- Register name to number mapping

#### main.c
- File format detection (assembly vs. machine code)
- Program loading from both formats
- Batch processing support
- Output file generation

#### simulate.h/c
- Single-cycle execution engine
- Pipelined execution with 5 stages
- Hazard detection and stall insertion
- Performance metric calculation
- Report generation

## Supported Instructions

The simulator supports the following MIPS instructions:

### R-Type (Register)
- `add $rd, $rs, $rt` - Addition
- `sub $rd, $rs, $rt` - Subtraction
- `and $rd, $rs, $rt` - Bitwise AND
- `or $rd, $rs, $rt` - Bitwise OR

### I-Type (Immediate)
- `addi $rt, $rs, imm` - Add immediate
- `lw $rt, offset($rs)` - Load word
- `sw $rt, offset($rs)` - Store word
- `beq $rs, $rt, label` - Branch if equal

### J-Type (Jump)
- `j label` - Unconditional jump

### Special
- `nop` - No operation

## Compilation

### Basic Compilation

```bash
gcc -o simulator main.c simulate.c decode.c CPUParameters.c -Wall -O2
```

### With Debug Symbols

```bash
gcc -o simulator main.c simulate.c decode.c CPUParameters.c -Wall -g
```

### Compilation Flags Explained

- `-o simulator` - Output executable named "simulator"
- `-Wall` - Enable all warning messages
- `-O2` - Optimization level 2 (recommended for performance)
- `-g` - Include debug symbols (for use with gdb)

## Usage

### Running a Single File

```bash
./simulator <input_file> [output_file]
```

**Examples:**
```bash
# Using default output file (output.txt)
./simulator program.asm

# Specifying custom output file
./simulator program.asm results.txt

# Running machine code
./simulator program_machine.asm my_results.txt
```

### Batch Processing

Process all predefined test files:

```bash
./simulator all
```

This processes the following files:
- `alu_heavy1.asm`
- `balanced_mix1.asm`
- `branchy_loop1.asm`
- `alu_heavy1_machine.asm`
- `balanced_mix1_machine.asm`
- `branchy_loop1_machine.asm`

Each generates:
- `<filename>_output.txt` - Performance report
- `<filename>_cycle_trace.txt` - Cycle-by-cycle execution trace

## Input File Formats

### Assembly Format (.asm)

```assembly
# Example MIPS assembly program
    addi $t0, $zero, 10    # Initialize counter
    addi $t1, $zero, 0     # Initialize sum
loop:
    add $t1, $t1, $t0      # sum = sum + counter
    addi $t0, $t0, -1      # counter--
    beq $t0, $zero, done   # if counter == 0, exit
    j loop                 # repeat
done:
    sw $t1, 0($zero)       # store result
```

**Features:**
- Comments with `#`
- Label support (e.g., `loop:`, `done:`)
- Register names (`$t0`, `$zero`, etc.) or numbers (`$8`, `$0`)
- Immediate values in decimal or hex

### Machine Code Format

```
0x20080005    # addi $t0, $zero, 5
0x20090000    # addi $t1, $zero, 0
0x01284820    # add $t1, $t1, $t0
0x2108ffff    # addi $t0, $t0, -1
```

**Features:**
- Hex values (0x prefix)
- One instruction per line
- Comments allowed

## Output Files

### Performance Report (`*_output.txt`)

Contains:
- Input file name and instruction listing
- Instruction type statistics
- **Single-Cycle Performance:**
  - Total cycles
  - Execution time
  - CPI (always 1.0)
- **Pipelined Performance:**
  - Total cycles (including stalls)
  - Stall and flush counts
  - Data and control hazard counts
  - Execution time
  - CPI (typically < 1.0 for efficient programs)
  - Speedup vs. single-cycle

### Cycle Trace (`*_cycle_trace.txt`)

Detailed cycle-by-cycle execution showing:
- Pipeline register contents
- Stall and flush events
- Hazard detection
- Branch outcomes

## Register Naming Conventions

The simulator supports both numeric and named registers:

| Name | Number | Purpose |
|------|--------|---------|
| `$zero` | 0 | Always zero |
| `$at` | 1 | Assembler temporary |
| `$v0-$v1` | 2-3 | Return values |
| `$a0-$a3` | 4-7 | Arguments |
| `$t0-$t7` | 8-15 | Temporaries |
| `$s0-$s7` | 16-23 | Saved temporaries |
| `$t8-$t9` | 24-25 | More temporaries |
| `$k0-$k1` | 26-27 | Kernel reserved |
| `$gp` | 28 | Global pointer |
| `$sp` | 29 | Stack pointer |
| `$fp` | 30 | Frame pointer |
| `$ra` | 31 | Return address |

## Pipeline Stages

The pipelined simulator implements a classic 5-stage MIPS pipeline:

1. **IF (Instruction Fetch)**: Fetch instruction from memory
2. **ID (Instruction Decode)**: Decode instruction and read registers
3. **EX (Execute)**: Perform ALU operation or calculate address
4. **MEM (Memory)**: Access data memory for loads/stores
5. **WB (Write Back)**: Write result back to register file

### Hazard Handling

- **Data Hazards**: Detected via register dependency checking
  - Stalls inserted when necessary
  - Forwarding used when available (configurable)
- **Control Hazards**: Branch decisions in EX stage
  - Pipeline flushed on taken branches
  - 2-cycle penalty for taken branches

## Performance Metrics

The simulator calculates:

- **Instruction Count**: Total instructions executed
- **Cycle Count**: Total clock cycles required
- **CPI**: Average cycles per instruction
- **Execution Time**: Based on configured clock period
- **Throughput**: Instructions per second
- **Speedup**: Pipelined performance / Single-cycle performance
- **Hazard Statistics**: Counts of data and control hazards

## Configuration

CPU parameters are initialized in `CPUParameters.c`:

```c
params->clock_rate_hz = 1e9;           // 1 GHz
params->pipeline_depth = 5;             // 5 stages
params->has_forwarding = 1;             // Enable forwarding
params->lw_ps = 800.0;                  // Load latency (ps)
params->sw_ps = 700.0;                  // Store latency (ps)
params->rtype_ps = 600.0;               // R-type latency (ps)
params->beq_ps = 500.0;                 // Branch latency (ps)
params->j_ps = 500.0;                   // Jump latency (ps)
```

## Troubleshooting

### Compilation Errors

**Missing simulate.c:**
```
gcc: error: simulate.c: No such file or directory
```
Ensure all source files are in the same directory.

**Undefined references:**
```
undefined reference to `run_single_cycle'
```
Make sure all `.c` files are included in the compilation command.

### Runtime Errors

**File not found:**
```
Failed to load program.asm
```
Check that input files are in the current directory or provide full paths.

**Invalid instruction:**
```
warning: invalid instruction at line X
```
Verify assembly syntax matches supported instruction set.

## Example Workflow

```bash
# 1. Compile the simulator
gcc -o simulator main.c simulate.c decode.c CPUParameters.c -Wall -O2

# 2. Create a test program (test.asm)
cat > test.asm << 'EOF'
    addi $t0, $zero, 10
    addi $t1, $zero, 0
loop:
    add $t1, $t1, $t0
    addi $t0, $t0, -1
    beq $t0, $zero, done
    j loop
done:
    nop
EOF

# 3. Run the simulator
./simulator test.asm test_output.txt

# 4. View results
cat test_output.txt

# 5. View trace (optional)
cat test.asm_cycle_trace.txt
```

## Limitations

- Maximum 1024 instructions per program
- 32 registers (standard MIPS)
- 256 words of data memory
- No floating-point instructions
- No multiplication/division
- No system calls
- Simplified memory model (no cache simulation)

## License

This code is provided for educational purposes.

## Authors
Computer Architecture Group 9
- Jonathan McChesney-Fleming, jgmcches@Cougarnet.uh.edu
- Martin Zachariah, mbzachar@cougarnet.uh.edu
- Hieu Nguyen, hxnguy21@cougarnet.uh.edu
- Remi Lokhandwala, ralokha2@cougarnet.uh.edu
- Tien Hoang, thoang28@cougarnet.edu.uh
