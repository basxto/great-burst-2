#include <gb/gb.h>
#include <gb/hardware.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <hUGEDriver.h>
#include "game.h"
#include "level.h"
#include "plonger.h"
#include "gfx.h"
#include "set_t_fix.h"

#define BALL_DIAMETER (12U)
#define BLOCK_HEIGHT (12U)
#define BLOCK_WIDTH (24U)
#define SCREEN_HEIGHT (144U)
#define SCREEN_WIDTH (160U)

// least and most significant bit can be checked cheaper
#define VERTICAL (0x1U)
#define HORIZONTAL (0x80U)

// index of the unbreakable block
#define OBSTACLE (5U)

// tilemaps
#define TM98 ((void*)0x9800)
#define TM9C ((void*)0x9c00)

// has position (x,y) and direction (dx,dy)
// if there is a block on left and right and ball 16x16, we have x max 128
typedef struct {
    uint8_t x;
    int8_t dx;
    uint8_t y;
    int8_t dy;
} Ball;
Ball ball;

typedef struct {
    uint8_t x;
    uint8_t width;
} Paddle;
Paddle paddle;

// these offsets are relevant for rendering
Level current_level;
// offsets relevant for the ball's collision detection
uint8_t current_real_offsets;
// gets used by interrupt_hack.s
// [0] is the variable, rest are offsets Y, X, Y, X
volatile uint8_t offset_array [(SCREEN_HEIGHT/12*4)+1];

bool demo;

uint8_t remaining_blocks;

void shrink_paddle(){
    uint8_t width = (paddle.width/8)-1;
    // the paddle
    fill_win_rect(width, 25, 1, 1, great_burst_bg_start+0x15);
    fill_win_rect(width, 26, 1, 1, great_burst_bg_start+0x16);
    --width;
    fill_win_rect(width, 25, 1, 1, great_burst_special_start+14);
    fill_win_rect(width, 26, 1, 1, great_burst_special_start+15);
    --width;
    if(width != 0){
        fill_win_rect(width, 25, 1, 1, great_burst_special_start+12);
        fill_win_rect(width, 26, 1, 1, great_burst_special_start+13);
    }
    paddle.x += 8/2;
    paddle.width -= 8;

}

void grow_paddle(){
    //TBD
}

void replace_block(uint8_t x, uint8_t y, uint8_t block){
    uint8_t tiles[6];

    uint8_t base = (0x80 - 6) + (block * 6);
    for(uint8_t i = 0; i < 6; ++i)
        tiles[i] = base++;

    // scale to tile size (24x16)
    x*=3;
    y*=2;
    // upper part
    set_tiles_fix(x, y, 3, 2, TM98, tiles);
    if(x < 4*3)
        set_tiles_fix(x+(6*3), y, 3, 2, TM98, tiles);
    else if(x == (LEVEL_WIDTH-1)*3)
        set_tiles_fix(32-(1*3), y, 3, 2, TM98, tiles);
    // lower part
    y+=8*2;
    x=(x+(LEVEL_WIDTH/2)*3)%(LEVEL_WIDTH*3);
    set_tiles_fix(x, y, 3, 2, TM98, tiles);
    if(x < 4*3)
        set_tiles_fix(x+(6*3), y, 3, 2, TM98, tiles);
    else if(x == (LEVEL_WIDTH-1)*3)
        set_tiles_fix(32-(1*3), y, 3, 2, TM98, tiles);
}

void transform_block(uint8_t x, uint8_t y){
    uint8_t block = current_level.map[x][y];
    uint8_t new_block = block;
    switch(block){
        case OBSTACLE:
            new_block = 4;
            // obstacles don't get counted
            ++remaining_blocks;
            break;
        case 2:
            new_block = 1;
            break;
        case 1:
            new_block = 2;
        default:
            break;
    }
    current_level.map[x][y] = new_block;
    replace_block(x, y, new_block);
}

// removes block from memory and screen
void remove_block(uint8_t x, uint8_t y){
    uint8_t block = current_level.map[x][y];
    uint8_t new_block = block;
    switch(block){
        case OBSTACLE:
            // TODO: play animation
            // we don't have to write back the same tile
            return;
            break;
        case 2:
            new_block = 1;
            break;
        case 4:
            new_block = 2;
            // transform blocks to the left and right
            if(x != 0)
                transform_block(x-1, y);
            else
                transform_block((LEVEL_WIDTH-1), y);

            if(x != (LEVEL_WIDTH-1))
                transform_block(x+1, y);
            else
                transform_block(0, y);
            break;
        case 3:
            // TODO: spawn blocks
        default:
            new_block = 0;
            if(remaining_blocks != 0)
                --remaining_blocks;
            break;
    }
    current_level.map[x][y] = new_block;
    replace_block(x, y, new_block);
}

// x,y of block to check
// adjusted x coordinate of ball
uint8_t collide_block(uint8_t x, uint8_t y, uint8_t ball_x){
    // we only have 8 rows of bricks
    if(y >= 8 || current_level.map[x][y] == 0)
        return 0;

    // offset by block width/height to avoid negative numbers
    // + ball radius to get ball center
    uint8_t relative_x = ball_x - x*BLOCK_WIDTH + BLOCK_WIDTH + (BALL_DIAMETER/2);
    uint8_t relative_y = ball.y - y*BLOCK_HEIGHT + BLOCK_HEIGHT + (BALL_DIAMETER/2);

    // break it down to circle-line intersections

    // get closest point on the edge
    uint8_t closest_x = (relative_x < BLOCK_WIDTH ? BLOCK_WIDTH : (relative_x > 2*BLOCK_WIDTH ? 2*BLOCK_WIDTH : relative_x));
    uint8_t closest_y = (relative_y < BLOCK_HEIGHT ? BLOCK_HEIGHT : (relative_y > 2*BLOCK_HEIGHT ? 2*BLOCK_HEIGHT : relative_y));

    // deltas
    uint8_t dx = abs(closest_x - relative_x);
    uint8_t dy = abs(closest_y - relative_y);
    // implicit integer promotion, since values can get too big
    if(dx*dx + dy*dy <= (BALL_DIAMETER/2)*(BALL_DIAMETER/2)){
        remove_block(x, y);
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
        if(paddle.width > 3*8)
            shrink_paddle();
    }else if(ball.y - ball.dy > SCREEN_HEIGHT-BALL_DIAMETER-16){
        // ball lost
        ball.dx = 0;
        ball.dy = 0;
        shrink_paddle();
    }

    volatile uint8_t row = ball.y / BLOCK_HEIGHT;
    //ball with row offset of first overlapping row
    volatile uint8_t ball_x1 = ball.x + offset_array[row*4+2];
    // and second row
    volatile uint8_t ball_x2 = ball.x + offset_array[(row+1)*4+2];

    mirror |= collide_block(ball_x1/BLOCK_WIDTH, row, ball_x1);
    mirror |= collide_block(ball_x1/BLOCK_WIDTH+1, row, ball_x1);
    mirror |= collide_block(ball_x2/BLOCK_WIDTH, row+1, ball_x1);
    mirror |= collide_block(ball_x2/BLOCK_WIDTH+1, row+1, ball_x1);

    // collide with paddle
    if(ball.y >= SCREEN_HEIGHT-10-BALL_DIAMETER-16 && ball.y <= SCREEN_HEIGHT-10-BALL_DIAMETER-16+2 && ball.dy < 0){
        if(ball.x + BALL_DIAMETER > paddle.x && ball.x < paddle.x + paddle.width){
            mirror |= VERTICAL;
            // avoid multiple collisions
            //ball.y = 144-10-12-16;
        }
    }

    if(mirror & HORIZONTAL)
        ball.dx *= -1;
    if(mirror & VERTICAL)
        ball.dy *= -1;

    //TODO: ??
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

    if(mirror != 0)
        plonger(2);
}

// render the struct to a sprite
// 8 on x are offscreen and 8 further the border
// 16 on y are offscreen and 16 further the border
void render_ball(){
    if(!demo){
        if(ball.dx == 0 && ball.dy == 0){
            ball.x = paddle.x + paddle.width/2 - BALL_DIAMETER/2;
            ball.y = SCREEN_HEIGHT-10-BALL_DIAMETER-16;
        }
    }else{
        paddle.x = ball.x - paddle.width/2 + BALL_DIAMETER/2;
        // catch negative numbers
        if(paddle.x > 200)
            paddle.x = 0;
        if(paddle.x > SCREEN_WIDTH - paddle.width - 16)
            paddle.x = SCREEN_WIDTH - paddle.width - 16;
    }
    move_sprite(0, ball.x+8+8, ball.y+16+16);
    move_sprite(1, ball.x+16+8, ball.y+16+16);
    move_sprite(2, ball.x+8+8, ball.y+24+16);
    move_sprite(3, ball.x+16+8, ball.y+24+16);
}

void render_paddle(){
    offset_array[10*4] = -paddle.x;
    offset_array[10*4+2] = -paddle.x;
    offset_array[11*4] = -paddle.x;
}

void render_level(){
    remaining_blocks = 0;
    uint8_t map[6];
    for(uint8_t x = 0; x < LEVEL_WIDTH; ++x){
        for(uint8_t y = 0; y < LEVEL_HEIGHT; ++y){
            uint8_t block = current_level.map[x][y];
            replace_block(x, y, block);
            if(block != 0 && block != OBSTACLE)
                ++remaining_blocks;

        }
    }
}

// set up game specific interrupts and such
void init_game(){
    uint8_t i;
    ball.x = 72-2*8;
    ball.dx = 0;
    ball.y = 110;
    ball.dy = 0;

    // use offset_array to store temporary tilemap
    // it gets reinitialized after this
    // this sets up the bottom of the background
    offset_array[0] = great_burst_bg_start;
    offset_array[1] = great_burst_bg_start+3;
    offset_array[2] = great_burst_bg_start+9;
    offset_array[3] = great_burst_bg_start+15;
    set_win_tiles(-1, 29, 1, 4, offset_array);
    offset_array[0] = great_burst_bg_start+2;
    offset_array[1] = great_burst_bg_start+8;
    offset_array[2] = great_burst_bg_start+14;
    offset_array[3] = great_burst_bg_start+17;
    set_win_tiles(18, 28, 1, 4, offset_array);

    // everything 0 first
    for(i = 0; i < (SCREEN_HEIGHT/12*4)+1; ++i){
        offset_array[i] = 0;
    }
    // setup Y scrolling
    for(i = 1; i < (8*4); i+=2){
        offset_array[i] = i+1-16;
    }
    // and for the bottom part
    for(i = (8*4)+1; i <= (SCREEN_HEIGHT/12*4)+1; i+=2){
        offset_array[i] = 9*8;
    }
    // setup border sprites
    for(uint8_t i = 4; i < 20; ++i){
        set_sprite_tile(i, great_burst_special_start+17);
        move_sprite(i, 8, i*8);
    }
    // setup border window
    fill_win_rect(0, 0, 1, 16, great_burst_special_start+17);
    move_win(SCREEN_WIDTH-1, 16);

    // setup background

    // top
    fill_win_rect(0, 28, 18, 1, great_burst_bg_start+1);
    fill_win_rect(0, 29, 18, 1, great_burst_bg_start+7);
    fill_win_rect(0, 30, 18, 1, great_burst_bg_start+13);
    fill_win_rect(0, 31, 18, 1, great_burst_bg_start+16);
    // bottom
    fill_win_rect(0, 23, 32, 1, great_burst_bg_start+0x12);
    fill_win_rect(0, 24, 32, 1, great_burst_bg_start+0x15);
    fill_win_rect(0, 25, 32, 1, great_burst_bg_start+0x15);
    fill_win_rect(0, 26, 32, 1, great_burst_bg_start+0x16);
    fill_win_rect(0, 27, 32, 1, great_burst_bg_start+0x14);


    // the paddle
    fill_win_rect(0, 25, 1, 1, great_burst_special_start+10);
    fill_win_rect(0, 26, 1, 1, great_burst_special_start+11);
    fill_win_rect(1, 25, 3, 1, great_burst_special_start+12);
    fill_win_rect(1, 26, 3, 1, great_burst_special_start+13);
    fill_win_rect(4, 25, 1, 1, great_burst_special_start+14);
    fill_win_rect(4, 26, 1, 1, great_burst_special_start+15);
    paddle.x = 42;
    paddle.width = 5*8;
}


// handles the ball
void ball_interrupt(){
    SCY_REG = -16;//23*8+6;
    SCX_REG = -8;
    LYC_REG = 16-1;
    //move_ball();
    //render_ball();
    offset_array[0] = 0;
    // set bg to 9C00
    LCDC_REG|=0x08U;
    // shoot ball
    uint8_t pad = joypad();
    if((pad&J_A || demo) && ball.dx == 0 && ball.dy == 0){
        ball.dx = 1;
        ball.dy = 2;
    }
    // move pad
    pad = pad & (J_LEFT | J_RIGHT);
    if(pad == J_LEFT && paddle.x != 0 && !demo){
        --paddle.x;
    }
    if(pad == J_RIGHT && paddle.x + paddle.width != SCREEN_WIDTH - 16 && !demo){
        ++paddle.x;
    }
}

inline void set_offset(uint8_t i, uint8_t offset){
    offset_array[i*4+2] += offset;
    offset_array[i*4+4] += offset;
    // switch half every 3 blocks
    if(offset_array[i*4+4] > 255 - 1*BLOCK_WIDTH){
        // jump back to the beginning of the row
        offset_array[i*4+4] += LEVEL_WIDTH/2*BLOCK_WIDTH;
        offset_array[i*4+2] += LEVEL_WIDTH/2*BLOCK_WIDTH;
        // switch to other half (Y offset)
        offset_array[i*4+1]+=16*8;
        offset_array[i*4+3]+=16*8;
    }else if(offset_array[i*4+4] > LEVEL_WIDTH/2*BLOCK_WIDTH){
        // jump back to the beginning of the row
        offset_array[i*4+4] -= LEVEL_WIDTH/2*BLOCK_WIDTH;
        offset_array[i*4+2] -= LEVEL_WIDTH/2*BLOCK_WIDTH;
        // switch to other half (Y offset)
        offset_array[i*4+1]+=16*8;
        offset_array[i*4+3]+=16*8;
    }
}

// load the blocks and such
void load_level(uint8_t lvl){
    init_game();
    demo = true;
    memcpy(current_level.map, level[lvl].map, LEVEL_HEIGHT*LEVEL_WIDTH);
    memcpy(current_level.offset, level[lvl].offset, 8);
    memcpy(current_level.speed, level[lvl].speed, 8);

    // copy offsets
    for(uint8_t i = 1; i < 8; ++i){
        offset_array[i*4+2] = current_level.offset[i-1];
        offset_array[i*4+4] = current_level.offset[i-1];
    }
    render_level();
    CRITICAL {
        LYC_REG = 16+5;
        STAT_REG = 0x40;
        remove_VBL(hUGE_dosound);
        add_VBL(ball_interrupt);
        add_VBL(hUGE_dosound);
    }
    set_interrupts(VBL_IFLAG | LCD_IFLAG);
    while(remaining_blocks != 0 && paddle.width != 8){
        render_paddle();
        // move block
        for(uint8_t i = 1; i < 8; ++i){
            uint8_t offset = current_level.speed[i-1] / 2;
            set_offset(i, offset);
        }
        wait_vbl_done();
        render_paddle();
        move_ball();
        render_ball();
        wait_vbl_done();
        render_paddle();
        // move block
        for(uint8_t i = 1; i < 8; ++i){
            uint8_t offset = current_level.speed[i-1] - (current_level.speed[i-1] / 2);
            set_offset(i, offset);
        }
        wait_vbl_done();
        render_paddle();
        move_ball();
        render_ball();
        wait_vbl_done();
    }
    // stop scrolling
    CRITICAL {
        STAT_REG = 0x00;
        remove_VBL(hUGE_dosound);
        remove_VBL(ball_interrupt);
        add_VBL(hUGE_dosound);
    }
    // reset bg to 9800
    LCDC_REG&=~0x08U;
    SCY_REG = 0;
    SCX_REG = -8;
    // clear map
    fill_bkg_rect(0, 0, 32, 32, ' ');
}