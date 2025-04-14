.segment "HEADER"
  .byte $4E, $45, $53, $1A  ; iNES header identifier
  .byte 2                   ; 2x 16KB PRG-ROM Banks
  .byte 1                   ; 1x  8KB CHR-ROM
  .byte $01, $00            ; mapper 0, vertical mirroring

.segment "STARTUP"

.segment "CHARS"

.segment "VECTORS"
  .addr nmi
  .addr reset
  .addr 0

.segment "CODE"

reset:
  sei
  ldx #$ff
  txs
  cli
  jmp Main
Main:
  ldx #$02
  lda $10,X ; load value $83 into A
@endlessLoop:
  jmp @endlessLoop

nmi:
  tay
  rti