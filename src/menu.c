#include <gb/gb.h>
#include <stdint.h>
#include "menu.h"
#include "font.h"
#include "game.h"

// max width that can be dislayed at a time
// without borders
#define LINE_WIDTH (uint8_t)(20U-2U)


const char text_main_menu[] = "START\nLEVEL\nRANDOM\nDEMO\nOPTIONS\nCREDITS";
const char text_main_menu_header[] = "MENU";
const char text_level_menu_header[] = "LEVEL";
const char text_cc0[] = "CC0";
const char text_ogaby3[] = "OGA-BY 3.0";
const char text_ccby3[] = "CC-BY 3.0";
const char text_by[] = "BY";
const char text_licensed[] = "LICENSED";
const char text_main_credit[] = "ALL CODE & EDITS BY";
const char text_basxto[] = " S.'BASXTO' RIEDEL";
const char text_buch_surt[] = "BUCH & SURT";
const char text_surt[] = "SURT";
const char text_shiru8bit[] = "SHIRU8BIT";
const char text_zombcool[] = "ZOMBCOOL";
const char text_breakout_set[] = "BREAKOUT SET";
const char text_breakdown[] = "BREAKDOWN";
const char text_hyperspace[] = "LOST IN HYPERSPACE";
const char text_bannerart[] = "BANNER ART";

typedef struct {
    uint8_t license;
    char *author;
    char *piece;
} Credit;

char buffer[LINE_WIDTH];

void menu_init(){
    // clear screen
    VBK_REG = 1;
    init_win(4);
    VBK_REG = 0;
    init_win(' ');
}

void wait_vbls(uint8_t amount){
    for(uint8_t i = amount; i!=0; --i)
        wait_vbl_done();
}

// blindly prints text
void window_print(char* str){

}

// prints an interactive choice dialog
// x is always set to 4 because of border and selector
uint8_t window_choice(uint8_t y, const char* str){
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
        wait_vbls(7);
        input = joypad();
    }
    return selection;
}

void credits(void){
    set_win_tiles(0, 0, sizeof(text_main_credit)-1, 1, text_main_credit);
    set_win_tiles(0, 1, sizeof(text_basxto)-1, 1, text_basxto);

    set_win_tiles(0, 3, sizeof(text_hyperspace)-1, 1, text_hyperspace);
    set_win_tiles(0, 4, sizeof(text_by)-1, 1, text_by);
    set_win_tiles(sizeof(text_by), 4, sizeof(text_shiru8bit)-1, 1, text_shiru8bit);
    set_win_tiles(0, 5, sizeof(text_licensed)-1, 1, text_licensed);
    set_win_tiles(sizeof(text_licensed), 5, sizeof(text_ccby3)-1, 1, text_ccby3);

    set_win_tiles(0, 7, sizeof(text_breakout_set)-1, 1, text_breakout_set);
    set_win_tiles(0, 8, sizeof(text_by)-1, 1, text_by);
    set_win_tiles(sizeof(text_by), 8, sizeof(text_buch_surt)-1, 1, text_buch_surt);
    set_win_tiles(0, 9, sizeof(text_licensed)-1, 1, text_licensed);
    set_win_tiles(sizeof(text_licensed), 9, sizeof(text_cc0)-1, 1, text_cc0);

    set_win_tiles(0, 11, sizeof(text_breakdown)-1, 1, text_breakdown);
    set_win_tiles(0, 12, sizeof(text_by)-1, 1, text_by);
    set_win_tiles(sizeof(text_by), 12, sizeof(text_surt)-1, 1, text_surt);
    set_win_tiles(0, 13, sizeof(text_licensed)-1, 1, text_licensed);
    set_win_tiles(sizeof(text_licensed), 13, sizeof(text_cc0)-1, 1, text_cc0);

    set_win_tiles(0, 15, sizeof(text_bannerart)-1, 1, text_bannerart);
    set_win_tiles(0, 16, sizeof(text_by)-1, 1, text_by);
    set_win_tiles(sizeof(text_by), 16, sizeof(text_zombcool)-1, 1, text_zombcool);
    set_win_tiles(0, 17, sizeof(text_licensed)-1, 1, text_licensed);
    set_win_tiles(sizeof(text_licensed), 17, sizeof(text_ogaby3)-1, 1, text_ogaby3);


    wait_vbls(7);
    while(!(joypad() & (J_A | J_B | J_START))){
        wait_vbl_done();
    }
}

void level_menu(void){
    set_win_tiles(4, 2, sizeof(text_level_menu_header)-1, 1, text_level_menu_header);
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
        set_win_tiles(4, 2, sizeof(text_main_menu_header)-1, 1, text_main_menu_header);
        uint8_t selection = window_choice(4, text_main_menu);
        menu_init();
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
                break;
            case 5:
                credits();
                break;
            default:
                break;
        }
        menu_init();
    }
    //HIDE_WIN;
    //return;
}