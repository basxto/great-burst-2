    .area    _LCDVEC_HEADER (ABS)
    .org    0x48        ; LCD
.int_LCD:
    jp    _block_bare_interrupt
    ; variables edfined in game.c
    .area _CODE
; hblank only lasts 85 to 208 dots (20 to 49 us) depending on previous mode 3 duration
; we are at 192/220
_block_bare_interrupt::
    push af
    push hl
    ; first element is a counter
    ld	hl, #_offset_array ; 12
    inc	(hl) ; 12
    ld a, a
    ; it's +1, so at least [1]
    ld	a, (hl) ; 8
    inc	(hl) ; 12
    ; add the offeset from [0]
    ; add uint8 to uint16
    add	a, l
    ld	l, a
    adc a, h
    sub l
    ld  h, a
    ; prepare next call
    ldh	a, (_LYC_REG)
    add	a, #0x6
    ldh	(_LYC_REG), a
    ; busy wait through mode 3 :/
1$:
    ldh	a, (_STAT_REG)
    rrca
    ; mode 3 has least significant bit 1
    jr 	C, 1$
    ; scroll Y accordingly
    ld  a, (hl+)
    ldh	(_SCY_REG), a
    ; and  X
    ld	a, (hl+)
    ldh	(_SCX_REG), a
    ; would take too long earlier
    pop hl
    pop af
    reti
    ; we always exit during mode 0 ;)