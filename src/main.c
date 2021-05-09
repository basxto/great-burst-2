#include <gb/gb.h>
#include <hUGEDriver.h>
#include <string.h>
#include "gfx.h"
#include "msx.h"
#include "game.h"
#include "sara.h"

const char text_press_start[] = "PRESS START!";
const char text_won[] = "YOU WON!";

// based on example from gbdk-2020
#define SGB_TRANSFER(A,B) map_buf[0]=(A),map_buf[1]=(B),sgb_transfer(map_buf)

void set_sgb_border(void){
    if (sgb_check()) {
        unsigned char map_buf[20];
        SGB_TRANSFER((SGB_MASK_EN << 3) | 1, 1);
        BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;
        SCX_REG = SCY_REG = 0U;
        UBYTE tmp_lcdc = LCDC_REG;

        HIDE_SPRITES, HIDE_WIN, SHOW_BKG;
        DISPLAY_ON;

        UBYTE i = 0U;
        for (UBYTE y = 0; y != 14U; ++y) {
            UBYTE * dout = map_buf;
            for(UBYTE x = 0U; x != 20U; ++x, *dout++ = i++);
            set_bkg_tiles(0, y, 20, 1, map_buf);
        }

        set_bkg_data(0, 64, great_burst_border_sgb_data);
        SGB_TRANSFER((SGB_CHR_TRN << 3) | 1, 0);
        set_bkg_data(0, 112, great_burst_border_sgb_map);
        set_bkg_data(128, 4, great_burst_border_sgb_pal);
        SGB_TRANSFER((SGB_PCT_TRN << 3) | 1, 0);

        // restore screen
        LCDC_REG = tmp_lcdc;
        fill_bkg_rect(0, 0, 20, 18, 0);
        SGB_TRANSFER((SGB_MASK_EN << 3) | 1, 0);
    }
}

void init(){
    // clear screen
    VBK_REG = 1;
    init_bkg(4);
    init_win(4);
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
    set_sprite_palette(0, 1, great_burst_blocks_cgb_pal+4*4);
    set_sprite_palette(1, 1, great_burst_blocks_cgb_pal+4*4);
    // replace fourth color for B version
    set_sprite_palette_entry(1, 3, great_burst_blocks_cgb_pal[4*4]);
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
    set_sgb_border();
    show_sara();
    init();
    while(1){
        for(uint8_t level = 2; level < 10; ++level){
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