#include <gb/gb.h>
#include <string.h>
#include "gfx.h"
#include "game.h"

const char text_press_start[] = "PRESS START!";

void init(){
    // init game elements
    set_sprite_data(0x20, great_burst_fg_size, great_burst_fg_data);
    set_bkg_data(0x80, great_burst_blocks_size, great_burst_blocks_data);
    // init menu font
    set_bkg_1bit_data(0x20, squont8ng_size, squont8ng_data, 3);
    set_bkg_tiles(0, 0, strlen(text_press_start), 1, text_press_start);
    SHOW_BKG;
}


int main(){
    init();
    waitpad(J_START);
    load_level(0);
    return 0;
}