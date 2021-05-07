#include <gb/gb.h>
#include <hUGEDriver.h>
#include <string.h>
#include "gfx.h"
#include "msx.h"
#include "game.h"
#include "sara.h"

const char text_press_start[] = "PRESS START!";
const char text_won[] = "YOU WON!";

void init(){
    // clear screen
    VBK_REG = 1;
    init_bkg(0);
    VBK_REG = 0;
    init_bkg(' ');
    // init menu font (we just filled the screen with spaces)
    set_bkg_1bit_data(squont8ng_start, squont8ng_size, squont8ng_data, 3);
    // enable sound on all channels
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
    // reset palette
    BGP_REG = 0xE4;
    // set nicer palettes etc
    cgb_compatibility();
    set_bkg_palette(great_burst_blocks_cgb_pal_index, great_burst_blocks_cgb_pal_amount, great_burst_blocks_cgb_pal);
    set_sprite_palette(0, 1, great_burst_blocks_cgb_pal);
    set_sprite_palette(1, 1, great_burst_blocks_cgb_pal);
    // replace fourth color for B version
    set_sprite_palette_entry(1, 3, great_burst_blocks_cgb_pal[0]);
    // init game elements
    set_sprite_data(great_burst_fg_start, great_burst_fg_size, great_burst_fg_data);
    set_sprite_data(great_burst_special_start, great_burst_special_size, great_burst_special_data);

    set_sprite_prop(0, S_PALETTE + 1);
    set_sprite_tile(0, great_burst_fg_start);
    set_sprite_prop(1, 0);
    set_sprite_tile(1, great_burst_fg_start + 1);
    set_sprite_prop(2, 0);
    set_sprite_tile(2, great_burst_fg_start + 2);
    set_sprite_prop(3, 0);
    set_sprite_tile(3, great_burst_fg_start + 3);
    VBK_REG = 1;
    // load cgb specific data
    set_bkg_data(great_burst_blocks_cgb_start, great_burst_blocks_cgb_size, great_burst_blocks_cgb_data);
    VBK_REG = 0;
    set_bkg_data(great_burst_blocks_start, great_burst_blocks_size, great_burst_blocks_data);
    set_bkg_data(great_burst_bg_start, great_burst_bg_size, great_burst_bg_data);
    // set dmg palette for shiny part
    OBP1_REG = 0x26;
    __critical {
        hUGE_init(&lost_in_hyperspace);
        add_VBL(hUGE_dosound);
    }
}


int main(){
    show_sara();
    init();
    while(1){
        for(uint8_t level = 0; level < 8; ++level){
            set_bkg_tiles(3, 8, sizeof(text_press_start)-1, 1, text_press_start);
            SHOW_BKG;
            waitpad(J_START);
            SHOW_SPRITES;
            SHOW_WIN;
            load_level(level);
            HIDE_WIN;
            HIDE_SPRITES;
            HIDE_BKG;
        }
        set_bkg_tiles(5, 8, sizeof(text_won)-1, 1, text_won);
        SHOW_BKG;
        waitpad(J_START);
    }
    return 0;
}