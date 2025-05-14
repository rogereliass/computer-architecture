; Sample program 2: Branch and control flow test
; Tests the branch and jump instructions, pipeline flushes

; Initialize counter and sum
ANDI R1, R0, 1     ; R1 = 1 (counter)
ANDI R2, R0, 0     ; R2 = 0 (sum)
ANDI R3, R0, 5     ; R3 = 5 (max count)

; Loop to sum the first 5 integers
loop_start:
  ADD R2, R2, R1   ; sum += counter
  ADD R1, R1, 1    ; counter++
  SUB R4, R1, R3   ; R4 = counter - max
  BEQZ R4, 1       ; If counter == max, exit loop
  ANDI R20, R0, 0  ; Set high byte to 0
  ANDI R21, R0, 3  ; Set low byte to 3 (loop_start address)
  JR R20, R21      ; Jump to loop_start (instruction at address 3)

; Test multiple branches in sequence  
ANDI R5, R0, 1     ; R5 = 1
BEQZ R5, 2         ; This branch should not be taken
ANDI R6, R0, 10    ; R6 = 10 (should be executed)
ANDI R7, R0, 20    ; R7 = 20 (should be executed)
BEQZ R0, 2         ; This branch should be taken (R0 is always 0)
ANDI R8, R0, 30    ; This should be skipped
ANDI R8, R0, 40    ; R8 = 40 (should be executed)

; Test pipeline filling after branch
ANDI R9, R0, 9     ; R9 = 9
ADD R10, R9, R9    ; R10 = 18

; End program
NOP 