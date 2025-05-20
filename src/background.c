#include "background.h"

// parallax scroling
fix16 offset_pos[SCREEN_TILES_H] = {0}; // 224 px / 8 px = 28
fix16 offset_speed[SCREEN_TILES_H] = {0};
s16 values[SCREEN_TILES_H] = {0};

////////////////////////////////////////////////////////////////////////////
// INIT

u16 BACKGROUND_init(u16 ind) {
	VDP_setPlaneSize(64, 64, TRUE);
	
	// PAL_setPalette(PAL_BACKGROUND, bg_mountains.palette->data, CPU);
	VDP_drawImageEx(BG_BACKGROUND, &bg_mountains, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
	
	VDP_setScrollingMode(HSCROLL_TILE , VSCROLL_COLUMN);
	
	set_offset_speed(0, 6, FIX16(0.05));
	set_offset_speed(15, 5, FIX16(-0.10));
	set_offset_speed(20, 8, FIX16(-0.20));
	
	// set the window to 1 to last row
	// VDP_setWindowVPos(true, 1);
	
	return bg_mountains.tileset->numTile;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void BACKGROUND_update() {
	for (u8 i = 0; i < SCREEN_TILES_H; i++) {
		// restart plane position when reaching screen width
		// if (offset_pos[i] > PLANE_W) {
		// 	offset_pos[i] -= PLANE_W;
		// }

		// store next offset in fix16
		offset_pos[i] += offset_speed[i];
		
		// cast to integer to input on VDP
		values[i] = fix16ToInt(offset_pos[i]);
	}

	VDP_setHorizontalScrollTile(BG_BACKGROUND, 0, values, SCREEN_TILES_H, DMA);
}

void set_offset_speed(u8 start, u8 len, fix16 speed) {
	if (start+len-1 >= SCREEN_TILES_H) {
		return;
	}
	for (u8 i = start; i <= start+len-1; i++) {
		offset_speed[i] = speed;
	}
}
