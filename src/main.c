#include <gb/gb.h>
#include <string.h>
#include "gfx.h"

const char text_press_start[] = "PRESS START!";

void init(){
    set_bkg_1bit_data(0x20, squont8ng_size, squont8ng_data, 3);
    set_bkg_tiles(0, 0, strlen(text_press_start), 1, text_press_start);
    SHOW_BKG;
}


int main(){
    init();
    waitpad(J_START);
    return 0;
}