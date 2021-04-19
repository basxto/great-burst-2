    .area    _LCDVEC_HEADER (ABS)

    .org    0x48        ; LCD
.int_LCD:
    jp    _block_bare_interrupt