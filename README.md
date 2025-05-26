# 🚀 Harvard Architecture CPU Simulator

A simulator for a 3-stage pipelined Harvard-architecture CPU with 12 instructions.

---

## 🧠 Architecture

- **Harvard Architecture**: Separate instruction and data memories
- **Instruction Memory**: 1024 words of 16 bits each, word-addressable (addresses `0–1023`)
- **Data Memory**: 2048 bytes, byte-addressable (addresses `0–2047`)
- **Register File**:
  - 64 general-purpose registers `R0–R63`, each 8 bits
  - `R0` is hardwired to zero; any write to R0 is silently ignored but logged
  - One status register `SREG` (8 bits); only bits `0–4` used:
    - Bit 0: `C` (carry)
    - Bit 1: `V` (overflow)
    - Bit 2: `N` (negative)
    - Bit 3: `S` (sign = N ⊕ V)
    - Bit 4: `Z` (zero)
  - One program counter `PC` (16 bits)

---

## 🔗 Simulations Video


[![ProStudent Simulation](https://img.youtube.com/vi/y6wt-EB4evw/0.jpg)](https://www.youtube.com/embed/y6wt-EB4evw)


---

## 🧾 Instruction Set

The CPU supports 12 instructions (opcodes `0–11`):

| Opcode | Instruction            | Description                                      | Flags Affected     |
|--------|------------------------|--------------------------------------------------|---------------------|
| 0      | **ADD rd, rs1, rs2**   | `rd ← rs1 + rs2`                                 | C, V, N, Z, S       |
| 1      | **SUB rd, rs1, rs2**   | `rd ← rs1 - rs2`                                 | V, N, Z, S          |
| 2      | **MUL rd, rs1, rs2**   | `rd ← rs1 × rs2`                                 | N, Z, S             |
| 3      | **ANDI rd, rs1, imm**  | `rd ← rs1 & imm`                                 | N, Z, S             |
| 4      | **EOR rd, rs1, rs2**   | `rd ← rs1 ^ rs2`                                 | N, Z, S             |
| 5      | **SAL rd, rs1, imm**   | `rd ← rs1 << imm`                                | N, Z, S             |
| 6      | **SAR rd, rs1, imm**   | `rd ← rs1 >> imm` (arithmetic shift)             | N, Z, S             |
| 7      | **LOAD rd, rs1, imm**  | `rd ← data_mem[rs1 + imm]`                       | —                   |
| 8      | **STORE rs1, rs2, imm**| `data_mem[rs1 + imm] ← rs2`                      | —                   |
| 9      | **BEQZ rs1, imm**      | if `rs1 == 0`, then `PC ← PC + 1 + imm`          | Flushes pipeline    |
| 10     | **JR R0, R1, _**       | `PC ← (R0 << 8) | R1`                            | Flushes pipeline    |
| 11     | **NOP**                | No operation                                     | —                   |

---

## 🔁 Pipeline

- **Stages**: `IF → ID → EX` (write-back is part of EX output latch)
- **Execution**:
  - Fully pipelined
  - Up to 3 instructions in flight at once
  - No hazard forwarding or stalling
  - On a **taken branch or jump**, the IF and ID stages are flushed by injecting **NOPs**

---

## 🛠️ Building and Running

```bash
# Build the simulator
make

# Run the simulator with a test program
./simulator3 tests/sample1.asm

# Run the tests
make test

```

## 🧪 Test Programs

The project includes two test programs:

1. **sample1.asm**: Tests arithmetic operations and flags
2. **sample2.asm**: Tests branch and control flow instructions

## 📤 Output

🔄 **For each cycle, the simulator outputs**:
- Cycle number
- Status of each pipeline stage (IF, ID, EX, WB)
- Any register writes or memory writes

🧾 **At termination, it dumps**:
- All GPR[0-63]
- SREG flags
- Final PC value
- Contents of instruction and data memory

---

## 📬 Contact
For questions or feedback, please contact [roger.elias669@gmail.com](mailto:roger.elias669@gmail.com).
