.segment "HEADER"
  .byte $4E, $45, $53, $1A  ; iNES header identifier
  .byte 2                   ; 2x 16KB PRG-ROM Banks
  .byte 1                   ; 1x  8KB CHR-ROM
  .byte $01, $00            ; mapper 0, vertical mirroring

.segment "STARTUP"

.segment "CHARS"

.segment "VECTORS"
  .addr 0
  .addr reset
  .addr 0

.segment "CODE"

reset:
  LDX #$0A              ; X = 10, operand 1
  STX $0000             ; mem($0000) = 10
  LDY #$03              ; Y = 3, operand 2
  LDA #$00              ; a = 0
loop:
  CLC
  ADC $0000
  DEY
  BNE loop              ; while (Y > 0): loop

Done:
    STA $0001           ; Store final result

Forever:
    JMP Forever            ; Infinite loop (halt)