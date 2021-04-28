#include <gb/gb.h>
#include <string.h>
#include "gfx.h"
#include "game.h"

const char text_press_start[] = "PRESS START!";
const char text_won[] = "YOU WON!";

void init(){
    // init game elements
    set_sprite_data(great_burst_fg_start, great_burst_fg_size, great_burst_fg_data);

    set_sprite_prop(0, S_PALETTE);
    set_sprite_tile(0, great_burst_fg_start);
    set_sprite_tile(1, great_burst_fg_start + 1);
    set_sprite_tile(2, great_burst_fg_start + 2);
    set_sprite_tile(3, great_burst_fg_start + 3);
    set_bkg_data(great_burst_blocks_start, great_burst_blocks_size, great_burst_blocks_data);
    // init menu font
    set_bkg_1bit_data(squont8ng_start, squont8ng_size, squont8ng_data, 3);
    // set dmg palette for shiny part
    OBP1_REG = 0x26;
}


int main(){
    init();
    while(1){
        for(uint8_t level = 0; level < 4; ++level){
            set_bkg_tiles(3, 8, sizeof(text_press_start)-1, 1, text_press_start);
            SHOW_BKG;
            waitpad(J_START);
            SHOW_SPRITES;
            load_level(level);
            HIDE_SPRITES;
            HIDE_BKG;
        }
        set_bkg_tiles(5, 8, sizeof(text_won)-1, 1, text_won);
        SHOW_BKG;
        waitpad(J_START);
    }
    return 0;
}