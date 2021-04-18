#ifndef LEVEL_H
#define LEVEL_H
#include <stdint.h>
#define LEVEL_WIDTH (6U)
#define LEVEL_HEIGHT (10U)
typedef uint8_t Level[LEVEL_HEIGHT][LEVEL_WIDTH];
extern const Level level[];
#endif