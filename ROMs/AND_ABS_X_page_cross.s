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
  ldx #$02
  lda #$55
  sta $0701
  lda #$AA
  and $06FF,X
@endlessLoop:
  jmp @endlessLoop