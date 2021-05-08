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
const uint8_t oga_spring_2021_gb_data[] = {
#include "../build/oga_spring_2021_gb.2bpp.cdata"
};
const uint8_t oga_spring_2021_cgb_data[] = {
#include "../build/oga_spring_2021_cgb.2bpp.cdata"
};
const uint8_t oga_spring_2021_overlay_cgb_data[] = {
#include "../build/oga_spring_2021_overlay_cgb.2bpp.cdata"
};
const uint8_t great_burst_border_sgb_data[] = {
#include "../build/great_burst_border_sgb.4bpp.cdata"
};

// image palettes (xxd is always 8b)
const uint8_t great_burst_blocks_cgb_pal[] = {
#include "../build/great_burst_blocks_cgb.pal.cdata"
};
const uint8_t oga_spring_2021_cgb_pal[] = {
#include "../build/oga_spring_2021_cgb.pal.cdata"
};
const uint8_t oga_spring_2021_overlay_cgb_pal[] = {
#include "../build/oga_spring_2021_overlay_cgb.pal.cdata"
};
const uint8_t great_burst_border_sgb_pal[] = {
#include "../build/great_burst_border_sgb.pal.cdata"
};

// image tilemaps (xxd is always 8b)
const uint8_t oga_spring_2021_gb_map[] = {
#include "../build/oga_spring_2021_gb.map.cdata"
};
const uint8_t oga_spring_2021_cgb_map[] = {
#include "../build/oga_spring_2021_cgb.map.cdata"
};
const uint8_t great_burst_border_sgb_map[] = {
#include "../build/great_burst_border_sgb.map.cdata"
};

// attribute maps (palett, flipping etc)
const uint8_t oga_spring_2021_cgb_attr[] = {
#include "../build/oga_spring_2021_cgb.attr.cdata"
};