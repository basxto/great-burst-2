#include <gb/gb.h>
#include <stdbool.h>
#include "game.h"


// has position (x,y) and direction (dx,dy)
// if there is a block on left and right and ball 16x16, we have x max 128
struct {
    uint8_t x;
    int8_t dx;
    uint8_t y;
    int8_t dy;
} ball;

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

// set up game specific interrupts and such
void init_game(){

}

// load the blocks and such
void load_level(uint8_t level){
    ball.x = 31;
    ball.dx = 1;
    ball.y = 33;
    ball.dy = 2;
    while(level == 0){
        move_ball();
        render_ball();
        wait_vbl_done();
        wait_vbl_done();
    }
}