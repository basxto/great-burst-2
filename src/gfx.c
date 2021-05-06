#pragma bank 1
#include <stdint.h>
// image data
const uint8_t squont8ng_data[] = {
#include "../build/squont8ng.1bpp.cdata"
};
const uint8_t great_burst_fg_data[] = {
#include "../build/great_burst_fg.2bpp.cdata"
};
const uint8_t great_burst_blocks_data[] = {
#include "../build/great_burst_blocks.2bpp.cdata"
};
const uint8_t great_burst_blocks_cgb_data[] = {
#include "../build/great_burst_blocks_cgb.2bpp.cdata"
};
const uint8_t great_burst_bg_data[] = {
#include "../build/great_burst_bg.2bpp.cdata"
};
const uint8_t great_burst_special_data[] = {
#include "../build/great_burst_special.2bpp.cdata"
};

// image palettes (xxd is always 8b)
const uint8_t great_burst_blocks_cgb_pal[] = {
#include "../build/great_burst_blocks_cgb.pal.cdata"
};