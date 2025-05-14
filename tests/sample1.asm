; Sample program 1: Tests arithmetic operations and flags
; Sets R1 = 5, R2 = 10, performs various operations and tests

; Initialize registers
ANDI R1, R0, 0      ; Clear R1
ANDI R2, R0, 0      ; Clear R2
ANDI R3, R0, 0      ; Clear R3

; Load values
ANDI R1, R0, 5      ; R1 = 5
ANDI R2, R0, 10     ; R2 = 10

; Test addition
ADD R3, R1, R2      ; R3 = R1 + R2 = 15

; Test subtraction
SUB R4, R2, R1      ; R4 = R2 - R1 = 5
SUB R5, R1, R2      ; R5 = R1 - R2 = -5 (0xFB in 2's complement)

; Test multiplication
MUL R6, R1, R2      ; R6 = R1 * R2 = 50 (0x32)

; Test logical operations
ANDI R7, R1, 3      ; R7 = R1 & 3 = 1
EOR R8, R1, R2      ; R8 = R1 ^ R2 = 15

; Test shifts
SAL R9, R1, 2       ; R9 = R1 << 2 = 20
SAR R10, R5, 1      ; R10 = R5 >> 1 = -3 (0xFD in 2's complement)

; Test memory operations
STORE R0, R1, 100   ; Store R1 at address 100
STORE R0, R2, 101   ; Store R2 at address 101
LOAD R11, R0, 100   ; Load from address 100 to R11
LOAD R12, R0, 101   ; Load from address 101 to R12

; Test write to R0 (should be ignored)
ADD R0, R1, R2      ; Try to write 15 to R0 (should remain 0)
LOAD R0, R0, 100    ; Try to write to R0 from memory

; Test branch
BEQZ R0, 2          ; Should branch (R0 is 0)
ADD R13, R1, R2     ; Should be skipped
ADD R13, R1, R1     ; R13 = R1 + R1 = 10

; Set up and test jump
ANDI R20, R0, 0     ; R20 = 0 (high byte of jump target)
ANDI R21, R0, 30    ; R21 = 30 (low byte of jump target)
JR R20, R21         ; Jump to address 0x001E (30)

; Instructions in the middle that should be skipped
ADD R30, R1, R2     ; Should be skipped
ADD R30, R2, R2     ; Should be skipped

; Target of the jump (at address 30)
jump_target:
ANDI R31, R0, 99    ; R31 = 99

; End program
NOP 