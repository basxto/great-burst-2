#include <gb/gb.h>
#include <gb/hardware.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "level.h"


// has position (x,y) and direction (dx,dy)
// if there is a block on left and right and ball 16x16, we have x max 128
typedef struct {
    uint8_t x;
    int8_t dx;
    uint8_t y;
    int8_t dy;
} Ball;
Ball ball;

Level current_level;
// gets used by interrupt_hack.s
// [0] is the variable, rest are offsets Y, X, Y, X
extern volatile uint8_t offset_array [(12*4)+1];

// ball is 12x12
// block is 12x24
// *4/3 => 16x16 16x32
// move and collide
void move_ball(){
    bool mirror_v = false, mirror_h = false;

    // bounce at screen borders
    if((uint8_t)ball.x < ball.dx){
        mirror_h = true;
        ball.x = 0;
    }else if(ball.x - ball.dx > 160-12-16){
        mirror_h = true;
        ball.x = 160-12-16;
    }
    if((uint8_t)ball.y < ball.dy){
        mirror_v = true;
        ball.y = 0;
    }else if(ball.y - ball.dy > 144-12-16){
        mirror_v = true;
        ball.y = 144-12-16;
    }

    // rescale from 12 to 16
    //volatile uint8_t ball_x16 = (uint8_t)(((uint16_t)ball.x*4U)/3U);
    //volatile uint8_t ball_y16 = (uint8_t)(((uint16_t)ball.y*4U)/3U);

    if(mirror_h)
        ball.dx *= -1;
    if(mirror_v)
        ball.dy *= -1;

    uint8_t pos_x = 0;
    uint8_t pos_y = 0;
    for(uint8_t x = 0; x < LEVEL_WIDTH; ++x){
        for(uint8_t y = 0; y < LEVEL_HEIGHT; ++y){
            pos_y += 12;
        }
        pos_y = 0;
        pos_x += 24;
    }
    ball.x -= ball.dx;
    ball.y -= ball.dy;
}

// render the struct to a sprite
void render_ball(){
    move_sprite(0, ball.x+8+8, ball.y+16+16);
    move_sprite(1, ball.x+16+8, ball.y+16+16);
    move_sprite(2, ball.x+8+8, ball.y+24+16);
    move_sprite(3, ball.x+16+8, ball.y+24+16);
}

void render_level(){
    uint8_t map[6];
    for(uint8_t x = 0; x < LEVEL_WIDTH; ++x){
        for(uint8_t y = 0; y < LEVEL_HEIGHT; ++y){
            uint8_t base = (0x80 - 6) + (current_level[x][y] * 6);
            for(uint8_t i = 0; i < 6; ++i)
                map[i] = base++;
            set_bkg_tiles(x*3+1, y*2+2, 3, 2, map);
        }
    }
}

// set up game specific interrupts and such
void init_game(){
    uint8_t i;
    ball.x = 31;
    ball.dx = 1;
    ball.y = 33;
    ball.dy = 2;
    offset_array[0] = 0;
    /*for(i = 1; i < (12*4); i+=2){
        offset_array[i] = 0;
    }*/
    for(i = 1; i < (12*4); ++i){
        offset_array[i] = i+1;
    }
    for(i = 2; i < (12*4); i+=2){
        offset_array[i] = 0;
    }
    // would be defined per level
    offset_array[4*4] = 12;
    offset_array[4*4+2] = 12;
    offset_array[6*4] = 12;
    offset_array[6*4+2] = 12;
}


// handles the ball
void ball_interrupt(){
    SCY_REG = 0;
    SCX_REG = 0;
    LYC_REG = 16+5;
    //move_ball();
    //render_ball();
    offset_array[0] = 0;
}

// load the blocks and such
void load_level(uint8_t lvl){
    init_game();
    memcpy(current_level, level[lvl], LEVEL_HEIGHT*LEVEL_WIDTH);
    render_level();
    CRITICAL {
        LYC_REG = 16+5;
        STAT_REG = 0x40;
        add_VBL(ball_interrupt);
    }
    set_interrupts(VBL_IFLAG | LCD_IFLAG);
    while(true){
        move_ball();
        render_ball();
        wait_vbl_done();
        wait_vbl_done();
    }
}