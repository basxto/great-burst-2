#ifndef LEVEL_H
#define LEVEL_H
#include <stdint.h>
#define LEVEL_WIDTH (6U)
#define LEVEL_HEIGHT (8U)
typedef uint8_t LevelMap[LEVEL_WIDTH][LEVEL_HEIGHT];
typedef struct {
    LevelMap map;
    int8_t offset[7];
    int8_t unused;
    int8_t speed[7];
    int8_t unused2;
} Level;
extern const Level level[];
#endif