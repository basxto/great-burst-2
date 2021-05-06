#include <gb/gb.h>
#include "sara.h"
#include "gfx.h"

void show_sara(void){
    // load palette
    BGP_REG = 0xE1;
    set_bkg_palette(oga_spring_2021_cgb_pal_index, oga_spring_2021_cgb_pal_amount, oga_spring_2021_cgb_pal);
    // load data
    if(_cpu == CGB_TYPE){
        set_bkg_data(0, 0x7f, oga_spring_2021_cgb_data);
        VBK_REG = 1;
        set_bkg_tiles(0, 0, 160/8, 144/8, oga_spring_2021_cgb_attr);
        VBK_REG = 0;
        set_bkg_tiles(0, 0, 160/8, 144/8, oga_spring_2021_cgb_map);
    }else{
        set_bkg_data(0, 0x7f, oga_spring_2021_gb_data);
        set_bkg_tiles(0, 0, 160/8, 144/8, oga_spring_2021_gb_map);
    }
    SHOW_BKG;
    waitpad(J_START);
}