# Harvard Architecture CPU Simulator

A simulator for a 3-stage pipelined Harvard-architecture CPU with 12 instructions.

## Architecture

- **Harvard Architecture**: Separate instruction and data memories
- **Instruction Memory**: 1024 words of 16 bits each, word-addressable (addresses 0 to 1023)
- **Data Memory**: 2048 bytes, byte-addressable (addresses 0 to 2047)
- **Register File**:
  - 64 general-purpose registers R0–R63, each 8 bits
  - R0 is hardwired to zero; any write to R0 is silently ignored but logged
  - One status register SREG (8 bits); bits 0–4 used:
    - Bit 0: C (carry)
    - Bit 1: V (overflow)
    - Bit 2: N (negative)
    - Bit 3: S (sign = N ⊕ V)
    - Bit 4: Z (zero)
  - One program counter PC (16 bits)

## Instruction Set

The CPU supports 12 instructions (opcodes 0-11):

- **ADD rd, rs1, rs2**: rd ← rs1 + rs2; sets C, V, N, Z, S
- **SUB rd, rs1, rs2**: rd ← rs1 – rs2; sets V, N, Z, S
- **MUL rd, rs1, rs2**: rd ← rs1 × rs2; sets N, Z, S
- **ANDI rd, rs1, imm**: rd ← rs1 & imm; sets N, Z, S
- **EOR rd, rs1, rs2**: rd ← rs1 ^ rs2; sets N, Z, S
- **SAL rd, rs1, imm**: rd ← rs1 << imm; sets N, Z, S
- **SAR rd, rs1, imm**: rd ← arithmetic‐right‐shift(rs1, imm); sets N, Z, S
- **LOAD rd, rs1, imm**: rd ← data_mem[ rs1 + imm ]; sets no flags
- **STORE rs1, rs2, imm**: data_mem[ rs1 + imm ] ← rs2; sets no flags
- **BEQZ rs1, imm**: if rs1==0 then PC ← PC_of_branch+1+imm; flush pipeline
- **JR R0,R1,ignored**: PC ← (R0 << 8) │ R1; flush pipeline
- **NOP**: does nothing

## Pipeline

- Three stages: IF → ID → EX → (WB combined with EX stage output latch)
- Fully pipelined, up to three instructions in flight
- No hazard forwarding or stalling
- On a taken branch or jump, flush IF and ID by inserting NOPs

## Building and Running

```bash
# Build the simulator
make

# Run the simulator with a test program
./simulator3 tests/sample1.asm

# Run the tests
make test
```

## Test Programs

The project includes two test programs:

1. **sample1.asm**: Tests arithmetic operations and flags
2. **sample2.asm**: Tests branch and control flow instructions

## Output

For each cycle, the simulator outputs:
- Cycle number
- Status of each pipeline stage (IF, ID, EX, WB)
- Any register writes or memory writes

At termination, it dumps:
- All GPR[0-63]
- SREG flags
- Final PC value
- Contents of instruction and data memory