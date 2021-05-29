#include <gb/gb.h>
#include <stdint.h>
#include "menu.h"
#include "font.h"
#include "game.h"

// max width that can be dislayed at a time
// without borders
#define LINE_WIDTH (uint8_t)(20U-2U)

char buffer[LINE_WIDTH];

void wait_vbls(uint8_t amount){
    for(uint8_t i = amount; i!=0; --i)
        wait_vbl_done();
}

// blindly prints text
void window_print(char* str){

}

// prints an interactive choice dialog
// x is always set to 4 because of border and selector
uint8_t window_choice(uint8_t y, char* str){
    uint8_t s;
    uint8_t items = 1;
    uint8_t selection = 0;
    uint8_t input = 0;
    // end of string not reached
    while(*str != '\0'){
        s = 0;
        // copy string
        while(*str != '\0' && *str != NEWLINE)
            buffer[s++] = *(str++);
        set_win_tiles(4, y++, s, 1, buffer);
        if(*str == NEWLINE){
            str++;
            items++;
        }
    }
    y-=items;
    while(input != J_A){
        switch(input){
            case J_UP:
                if(selection == 0)
                    selection = items-1;
                else
                    selection--;
                break;
            case J_DOWN:
                if(selection == items-1)
                    selection = 0;
                else
                    selection++;
                break;
            default:
                break;
        }
        fill_win_rect(2, y, 1, items, SPACE);
        set_win_tile_xy(2, y+selection, SELECTOR);
        wait_vbls(5);
        input = joypad();
    }
    return selection;
}

void level_menu(void){
    set_win_tiles(4, 2, 5, 1, "LEVEL");
    uint8_t selection = window_choice(4, "1\n2\n3\n4\n5");
    SHOW_SPRITES;
    load_level(selection, false);
    HIDE_SPRITES;
    HIDE_BKG;
    move_win(7, 0);
}

void main_menu(void){
    SHOW_WIN;
    while(1){
        set_win_tiles(4, 2, 4, 1, "MENU");
        uint8_t selection = window_choice(4, "START\nLEVEL\nRANDOM\nDEMO\nOPTIONS\nCREDITS");
        switch(selection){
            case 0:
                SHOW_SPRITES;
                for(uint8_t level = 0; level < 10; ++level){
                    if(load_level(level, false) == 0)
                        break; // player lost
                }
                HIDE_SPRITES;
                HIDE_BKG;
                move_win(7, 0);
                break;
            case 1:
                level_menu();
                break;
            case 3:
                SHOW_SPRITES;
                load_level(2, true);
                load_level(4, true);
                load_level(6, true);
                HIDE_SPRITES;
                HIDE_BKG;
                move_win(7, 0);
            default:
                break;
        }
    }
    //HIDE_WIN;
    //return;
}