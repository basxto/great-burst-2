#include <gb/gb.h>
#include <stdint.h>
#include "plonger.h"

// From original Great Burst
// Basically like copying assets,
//  since I want to keep the sound

#define note_c 0x00
#define note_cis 0x01
#define note_des 0x01
#define note_d 0x02
#define note_dis 0x03
#define note_es 0x03
#define note_e 0x04
#define note_f 0x05
#define note_fis 0x06
#define note_ges 0x06
#define note_g 0x07
#define note_gis 0x08
#define note_as 0x08
#define note_a 0x09
#define note_ais 0x0A
#define note_b 0x0A
#define note_h 0x0B

// value taken from XPMCK
// probably octaves Great to Five-lined
const uint16_t notes[][12] = {{0x002C, 0x009C, 0x0106, 0x016A, 0x01C9, 0x0222,
                              0x0276, 0x02C6, 0x0311, 0x0358, 0x039B, 0x03DA},
                             {0x0416, 0x044E, 0x0483, 0x04B5, 0x04E4, 0x0511,
                              0x053B, 0x0563, 0x0588, 0x05AC, 0x05CD, 0x05ED},
                             {0x060B, 0x0627, 0x0641, 0x065A, 0x0672, 0x0688,
                              0x069D, 0x06B1, 0x06C4, 0x06D6, 0x06E6, 0x06F6},
                             {0x0705, 0x0713, 0x0720, 0x072D, 0x0739, 0x0744,
                              0x074E, 0x0758, 0x0762, 0x076B, 0x0773, 0x077B},
                             {0x0782, 0x0789, 0x0790, 0x0796, 0x079C, 0x07A2,
                              0x07A7, 0x07AC, 0x07B1, 0x07B5, 0x07B9, 0x07BD},
                             {0x07C1, 0x07C4, 0x07C8, 0x07CB, 0x07CE, 0x07D1,
                              0x07D3, 0x07D6, 0x07D8, 0x07DA, 0x07DC, 0x07DE},
                             {0x07E0, 0x07E2, 0x07E4, 0x07E5, 0x07E7, 0x07E8,
                              0x07E9, 0x07EB, 0x07EC, 0x07ED, 0x07EE, 0x07EF}};


void plonger(uint8_t note) {
    NR10_REG = 0x13; // arpeggio | 3
    NR11_REG = 0x50; // 50% duty
    NR12_REG = 0xF7; // volume envelope
    switch (note) {
    case 0:                                                   // break
        NR14_REG = 0xC0 | ((notes[0][note_fis] >> 8) & 0x07); // msb
        NR13_REG = notes[0][note_fis] & 0xFF;
        break;
    case 1:                                                   // paddle
        NR14_REG = 0xC0 | ((notes[0][note_gis] >> 8) & 0x07); // msb
        NR13_REG = notes[0][note_gis] & 0xFF;
        break;
    case 3:                                                 // shoot
        NR10_REG = 0x14;                                    // arpeggio | 4
        NR14_REG = 0xC0 | ((notes[0][note_h] >> 8) & 0x07); // msb
        NR13_REG = notes[0][note_h] & 0xFF;
        break;
    case 4:                                                 // die
        NR10_REG = 0x15;                                    // arpeggio | 5
        NR14_REG = 0xC0 | ((notes[1][note_d] >> 8) & 0x07); // msb
        NR13_REG = notes[1][note_d] & 0xFF;
        break;
    case 5:              // cursor
        NR11_REG = 0x90; // 75% duty
        NR10_REG = 0x12; // arpeggio | 0
        // NR12_REG = 0xF1; // volume envelope
        NR14_REG = 0xC0 | ((notes[0][note_e] >> 8) & 0x07); // msb
        NR13_REG = notes[0][note_e] & 0xFF;
        break;
    case 6:                                                 // select
        NR11_REG = 0x90;                                    // 75% duty
        NR10_REG = 0x13;                                    // arpeggio | 4
        NR14_REG = 0xC0 | ((notes[0][note_h] >> 8) & 0x07); // msb
        NR13_REG = notes[0][note_h] & 0xFF;
        break;
    case 2:
    default:                                                // wall
        NR14_REG = 0xC0 | ((notes[0][note_h] >> 8) & 0x07); // msb
        NR13_REG = notes[0][note_h] & 0xFF;
    }
}