#include <gb/gb.h>
#include <gb/hardware.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "level.h"

#define BALL_DIAMETER (12U)
#define BLOCK_HEIGHT (12U)
#define BLOCK_WIDTH (24U)
#define SCREEN_HEIGHT (144U)
#define SCREEN_WIDTH (160U)

// least and most significant bit can be checked cheaper
#define VERTICAL (0x1U)
#define HORIZONTAL (0x80U)

// has position (x,y) and direction (dx,dy)
// if there is a block on left and right and ball 16x16, we have x max 128
typedef struct {
    uint8_t x;
    int8_t dx;
    uint8_t y;
    int8_t dy;
} Ball;
Ball ball;

// these offsets are relevant for rendering
Level current_level;
// offsets relevant for the ball's collision detection
uint8_t current_real_offsets;
// gets used by interrupt_hack.s
// [0] is the variable, rest are offsets Y, X, Y, X
volatile uint8_t offset_array [(SCREEN_HEIGHT/12*4)+1];

// x,y of block to check
// adjusted x coordinate of ball
uint8_t collide_block(uint8_t x, uint8_t y, uint8_t ball_x){
    // we only have 8 rows of bricks
    if(y >= 8 || current_level.map[x][y] == 0)
        return 0;

    // offset by block width/height to avoid negative numbers
    // + ball radius to get ball center
    uint8_t relative_x = ball_x - x*24 + 24 + 6;
    uint8_t relative_y = ball.y - y*12 + 12 + 6;

    // break it down to circle-line intersections

    // get closest point on the edge
    uint8_t closest_x = (relative_x < 24 ? 24 : (relative_x > 2*24 ? 2*24 : relative_x));
    uint8_t closest_y = (relative_y < 12 ? 12 : (relative_y > 2*12 ? 2*12 : relative_y));

    // deltas
    uint8_t dx = abs(closest_x - relative_x);
    uint8_t dy = abs(closest_y - relative_y);
    // implicit integer promotion, since values can get too big
    if(dx*dx + dy*dy <= 6*6){
        dy *= 2; // make the 24x12 rectangle a square
        if(dx == dy)
            return VERTICAL | HORIZONTAL;
        else if(dx > dy)
            return HORIZONTAL;
        else
            return VERTICAL;
    }

    return 0;
}

// ball is 12x12
// block is 12x24
// *4/3 => 16x16 16x32
// move and collide
void move_ball(){
    uint8_t mirror = 0;

    // bounce at screen borders
    if((uint8_t)ball.x < ball.dx){
        mirror |= HORIZONTAL;
        ball.x = 0;
    }else if(ball.x - ball.dx > SCREEN_WIDTH-BALL_DIAMETER-16){
        mirror |= HORIZONTAL;
        ball.x = SCREEN_WIDTH-BALL_DIAMETER-16;
    }
    if((uint8_t)ball.y < ball.dy){
        mirror |= VERTICAL;
        ball.y = 0;
    }else if(ball.y - ball.dy > SCREEN_HEIGHT-BALL_DIAMETER-16){
        mirror |= VERTICAL;
        ball.y = SCREEN_HEIGHT-BALL_DIAMETER-16;
    }

    volatile uint8_t row = ball.y / 12;
    //ball with row offset of first overlapping row
    volatile uint8_t ball_x1 = ball.x + offset_array[row*4];
    // and second row
    volatile uint8_t ball_x2 = ball.x + offset_array[(row+1)*4];

    mirror |= collide_block(ball_x1/24, row, ball_x1);
    mirror |= collide_block(ball_x1/24+1, row, ball_x1);
    mirror |= collide_block(ball_x2/24, row+1, ball_x1);
    mirror |= collide_block(ball_x2/24+1, row+1, ball_x1);

    if(mirror & HORIZONTAL)
        ball.dx *= -1;
    if(mirror & VERTICAL)
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
// 8 on x are offscreen and 8 further the border
// 16 on y are offscreen and 16 further the border
void render_ball(){
    move_sprite(0, ball.x+8+8, ball.y+16+16);
    move_sprite(1, ball.x+16+8, ball.y+16+16);
    move_sprite(2, ball.x+8+8, ball.y+24+16);
    move_sprite(3, ball.x+16+8, ball.y+24+16);
}

void render_level(){
    uint8_t map[6];
    for(uint8_t x = 0; x < LEVEL_WIDTH; ++x){
        for(uint8_t y = LEVEL_HEIGHT; y > 0; --y){
            uint8_t base = (0x80 - 6) + (current_level.map[x][y-1] * 6);
            for(uint8_t i = 0; i < 6; ++i)
                map[i] = base++;
            set_bkg_tiles(x*3, y*2, 3, 2, map);
            if(x < 4)
                set_bkg_tiles((x+6)*3, y*2, 3, 2, map);
        }
    }
}

// set up game specific interrupts and such
void init_game(){
    uint8_t i;
    ball.x = 31;
    ball.dx = 1;
    ball.y = 95;
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
}


// handles the ball
void ball_interrupt(){
    SCY_REG = 0;
    SCX_REG = -8;
    LYC_REG = 16+5;
    //move_ball();
    //render_ball();
    offset_array[0] = 0;
}

// load the blocks and such
void load_level(uint8_t lvl){
    init_game();
    memcpy(current_level.map, level[lvl].map, LEVEL_HEIGHT*LEVEL_WIDTH);
    memcpy(current_level.offset, level[lvl].offset, 8);
    memcpy(current_level.speed, level[lvl].speed, 8);

    // copy offsets
    for(uint8_t i = 1; i < 8; ++i){
        offset_array[i*4] = current_level.offset[i-1];
        offset_array[i*4+2] = current_level.offset[i-1];
    }
    render_level();
    CRITICAL {
        LYC_REG = 16+5;
        STAT_REG = 0x40;
        add_VBL(ball_interrupt);
    }
    set_interrupts(VBL_IFLAG | LCD_IFLAG);
    while(true){
        // move block
        for(uint8_t i = 1; i < 8; ++i){
            uint8_t offset = current_level.speed[i-1] / 2;
            offset_array[i*4] += offset;
            offset_array[i*4+2] += offset;
        }
        wait_vbl_done();
        move_ball();
        render_ball();
        wait_vbl_done();
        // move block
        for(uint8_t i = 1; i < 8; ++i){
            uint8_t offset = current_level.speed[i-1] - (current_level.speed[i-1] / 2);
            offset_array[i*4] += offset;
            offset_array[i*4+2] += offset;
        }
        wait_vbl_done();
        move_ball();
        render_ball();
        wait_vbl_done();
    }
}