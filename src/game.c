#include <gb/gb.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "level.h"


// has position (x,y) and direction (dx,dy)
// if there is a block on left and right and ball 16x16, we have x max 128
struct {
    uint8_t x;
    int8_t dx;
    uint8_t y;
    int8_t dy;
} ball;

Level current_level;

// move and collide
void move_ball(){
    bool mirror_v = false, mirror_h = false;
    // bounce at screen borders
    if((uint8_t)ball.x < ball.dx){
        mirror_h = true;
        ball.x = 0;
    }else if(ball.x - ball.dx > 160-16){
        mirror_h = true;
        ball.x = 160-16;
    }
    if((uint8_t)ball.y < ball.dy){
        mirror_v = true;
        ball.y = 0;
    }else if(ball.y - ball.dy > 144-16){
        mirror_v = true;
        ball.y = 144-16;
    }
    if(mirror_h)
        ball.dx *= -1;
    if(mirror_v)
        ball.dy *= -1;
    ball.x -= ball.dx;
    ball.y -= ball.dy;
}

// render the struct to a sprite
void render_ball(){
    move_sprite(0, ball.x+8, ball.y+16);
    move_sprite(1, ball.x+16, ball.y+16);
    move_sprite(2, ball.x+8, ball.y+24);
    move_sprite(3, ball.x+16, ball.y+24);
}

void render_level(){
    uint8_t map[6];
    for(uint8_t y = 0; y < LEVEL_HEIGHT; ++y){
        for(uint8_t x = 0; x < LEVEL_WIDTH; ++x){
            uint8_t base = (0x80 - 6) + (current_level[y][x] * 6);
            for(uint8_t i = 0; i < 6; ++i)
                map[i] = base++;
            set_bkg_tiles(x*3+1, y*2+1, 3, 2, map);
        }
    }
}

// set up game specific interrupts and such
void init_game(){

}

// load the blocks and such
void load_level(uint8_t lvl){
    ball.x = 31;
    ball.dx = 1;
    ball.y = 33;
    ball.dy = 2;
    memcpy(current_level, level[lvl], LEVEL_HEIGHT*LEVEL_WIDTH);
    render_level();
    while(true){
        move_ball();
        render_ball();
        //render_level();
        wait_vbl_done();
        wait_vbl_done();
    }
}