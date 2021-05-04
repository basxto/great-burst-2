#ifndef SET_T_FIX_H
#define SET_T_FIX_H
#include <stdint.h>
void set_tiles_fix(uint8_t x,
                   uint8_t y,
                   uint8_t w,
                   uint8_t h,
                   uint8_t *vram_addr,
                   const uint8_t *tiles) __nonbanked __preserves_regs(b, c);
#endif