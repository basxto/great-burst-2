#include <gb/gb.h>
#include "sara.h"
#include "gfx.h"

void show_sara(void){
    // load palette
    BGP_REG = 0xE1;
    set_bkg_palette(oga_spring_2021_cgb_pal_index, oga_spring_2021_cgb_pal_amount, oga_spring_2021_cgb_pal);
    set_sprite_palette(oga_spring_2021_overlay_cgb_pal_index, oga_spring_2021_overlay_cgb_pal_amount, oga_spring_2021_overlay_cgb_pal);
    // load data
    if(_cpu == CGB_TYPE){
        set_bkg_data(oga_spring_2021_cgb_data_start, oga_spring_2021_cgb_data_size, oga_spring_2021_cgb_data);
        set_sprite_data(oga_spring_2021_overlay_cgb_data_start, oga_spring_2021_overlay_cgb_data_size, oga_spring_2021_overlay_cgb_data);
        VBK_REG = 1;
        set_bkg_tiles(0, 0, 160/8, 144/8, oga_spring_2021_cgb_attr);
        VBK_REG = 0;
        set_bkg_tiles(0, 0, 160/8, 144/8, oga_spring_2021_cgb_map);
        // arrange the sprites
        for(uint8_t i = 0; i < oga_spring_2021_overlay_cgb_data_size; ++i){
            uint8_t  x = (i%7)*8;
            uint8_t  y = (i/7)*8;
            set_sprite_tile(i, oga_spring_2021_overlay_cgb_data_start + i);
            if(x < 16){
                set_sprite_prop(i, 0);
                x+=67+8;
                y+=61+16;
            }else{
                set_sprite_prop(i, 1);
                x+=85+8;
                y+=57+16;
            }
            if(x > 85+8+24){
                set_sprite_prop(i, 2);
                x-=8;
            }
            if(i==0){
                x+=2;
            }
            move_sprite(i, x, y);
        }
    }else{
        set_bkg_data(oga_spring_2021_gb_data_start, oga_spring_2021_gb_data_size, oga_spring_2021_gb_data);
        set_bkg_tiles(0, 0, 160/8, 144/8, oga_spring_2021_gb_map);
    }
    SHOW_BKG;
    SHOW_SPRITES;
    waitpad(J_START);
    // hide sprites again
    for(uint8_t i = 0; i < oga_spring_2021_overlay_cgb_data_size; ++i)
        move_sprite(i, 0, 0);
}