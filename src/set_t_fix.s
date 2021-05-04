.globl .set_xy_tt

.area	_BASE

_set_tiles_fix::
    ; callee saves bc
    push    bc
    call    1$
    pop     bc
    ret
1$:
    ldhl    sp, #6
    ; x
    ld      a, (hl+)
    ld      d, a
    ; y
    ld      a, (hl+)
    ld      e, a
    ; w
    ld      a, (hl+)
    ; h
    ld      l, (hl)
    ld      h, a
    ; has to be below return value
    push    hl
    ldhl    sp, #15
    ; tiles
    ld      a, (hl-)
    ld      b, a
    ld      a, (hl-)
    ld      c, a
    ; vram_addr
    ld      a, (hl-)
    ld      l, (hl)
    ld      h, a
    jp      .set_xy_tt