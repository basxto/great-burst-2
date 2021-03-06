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
    push bc
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
    ld  c, #<(_LYC_REG)
    ldh	a, (c)
    add	a, #0x6
    ldh	(c), a
    ; fetch values
    ld  a, (hl+)
    ld	h, (hl)
    ld	l, a
    ; move it additionally to left by a tile
    ld  a, #-8
    add h
    ld  h, a
    ; switch to the other tilemap for certain parts
    ldh	a, (c) ; _LYC_REG
    ; change tilemap only for scan lines>112
    ld  b, #0
    cp  a, #112
    jr  c, 2$
    ld  b, #0b00001000
2$:
    ldh a, (_LCDC_REG)
    and a, #0b11110111
    or  a, b
    ld  b, a
    ; ^^^ this part can be slower than a short mode 2 + 3
    ; but we don't care since we have more time in mode 0 then

    ; busy wait through mode 3 :/ (immediately in mode 0)
    ld  c, #<(_STAT_REG)
1$:
    ldh	a, (c)
    rrca
    ; mode 3 has least significant bit 1
    jr 	C, 1$
    ; scroll Y accordingly
    ld  a, l
    ldh	(_SCY_REG), a
    ; and  X
    ld	a, h
    ldh	(_SCX_REG), a
    ; and lcd setting
    ld  a, b
    ldh (_LCDC_REG), a
    ; would take too long earlier
    pop bc
    pop hl
    pop af
    reti
    ; we always exit during mode 0 ;)