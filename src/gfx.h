#ifndef GFX_H
#define GFX_H
#include <stdint.h>
extern const uint8_t squont8ng_data[];
#define squont8ng_size (16U*5U)
#define squont8ng_start (0x20U)
extern const uint8_t great_burst_fg_data[];
#define great_burst_fg_size (6U*4U)
#define great_burst_fg_start (0x20U)
extern const uint8_t great_burst_blocks_data[];
#define great_burst_blocks_size (13U*6U)
#define great_burst_blocks_start (0x80U)
extern const uint8_t great_burst_blocks_cgb_data[];
#define great_burst_blocks_cgb_size (6U*6U)
#define great_burst_blocks_cgb_start (0x80U)
extern const uint8_t great_burst_bg_data[];
#define great_burst_bg_size (23U)
#define great_burst_bg_start (0x1U)
extern const uint8_t great_burst_special_data[];
#define great_burst_special_size (26U)
#define great_burst_special_start (0xffU-great_burst_special_size+1)

extern const uint16_t great_burst_blocks_cgb_pal[];
#define great_burst_blocks_cgb_pal_amount (5U)
#define great_burst_blocks_cgb_pal_index (0U)
#endif