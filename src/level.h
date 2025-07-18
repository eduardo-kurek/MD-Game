#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <genesis.h>
#include "globals.h"
#include "gameobject.h"
#include "resources.h"

#define NUMBER_OF_LEVELS 5

#define IDX_EMPTY 	   39
#define IDX_WALL_FIRST 0
#define IDX_WALL_LAST  24
#define IDX_BOTTOM_THORN 32
#define IDX_TOP_THORN 30
#define IDX_LEFT_THORN 34
#define IDX_RIGHT_THORN 28
#define IDX_JUMP_REFRESH 26
#define IDX_CHECKPOINT 36
#define IDX_TRAMPOLINE 38
#define IDX_IS_THORN(x) ((x == IDX_BOTTOM_THORN) || (x == IDX_TOP_THORN) || (x == IDX_LEFT_THORN) || (id == IDX_RIGHT_THORN))
#define IDX_IS_JUMP_REFRESH(x) (x == IDX_JUMP_REFRESH)
#define IDX_IS_CHECKPOINT(x) (x == IDX_CHECKPOINT)
#define IDX_IS_TRAMPOLINE(x) (x == IDX_TRAMPOLINE)
#define IDX_IS_WALL(x) ((x >= IDX_WALL_FIRST) && (x <= IDX_WALL_LAST))

#define LAST_ROOM 15

#define OFFSCREEN_TILES 3

#define COLLISION_LEFT   0b0001
#define COLLISION_RIGHT  0b0010
#define COLLISION_HORIZ  0b0011

#define COLLISION_TOP    0b0100
#define COLLISION_BOTTOM 0b1000
#define COLLISION_VERT   0b1100

extern Map* map;
extern u8 collision_map[SCREEN_METATILES_W + OFFSCREEN_TILES*2][SCREEN_METATILES_H + OFFSCREEN_TILES*2]; // screen collision map
extern u16 tilemap_buff[SCREEN_TILES_W * SCREEN_TILES_H];

extern u8 collision_result;
extern char text[5];

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 LEVEL_init(u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

inline u8 LEVEL_collision_result() {
	return collision_result;
}

inline u8 LEVEL_wallXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES] == 1;
}

inline u8 LEVEL_tileXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES];
}

inline u8 LEVEL_tileIDX16(s16 metatile_x, s16 metatile_y) {
	return collision_map[metatile_x + OFFSCREEN_TILES][metatile_y + OFFSCREEN_TILES];
}

inline u16 LEVEL_mapbuffIDX8(s16 tile_x, s16 tile_y) {
	return (tilemap_buff[tile_y * SCREEN_TILES_W + tile_x]  - map->baseTile) & TILE_INDEX_MASK;
 }

inline void LEVEL_set_mapbuffIDX8(s16 tile_x, s16 tile_y, u16 value) {
	tilemap_buff[tile_y * SCREEN_TILES_W + tile_x] = value + map->baseTile;
}

/**
 * Define valor para tile no mapa de colisão.
 * @param x Posição X em pixels.
 * @param y Posição Y em pixels.
 */
static inline void LEVEL_set_tileXY(s16 x, s16 y, u8 value) {
	collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES] = value;
}

/**
 * Define valor para tile no mapa de colisão.
 * @param x Posição X em metatiles 16x16.
 * @param y Posição Y em metatiles 16x16.
 */
static inline void LEVEL_set_tileIDX16(s16 x, s16 y, u8 value) {
	collision_map[x + OFFSCREEN_TILES][y + OFFSCREEN_TILES] = value;
}

u8 LEVEL_check_wall(GameObject* obj);
void LEVEL_move_and_slide(GameObject* obj);

void LEVEL_remove_tileXY(s16 x, s16 y);

/**
 * 	map_tiles : 0 | 1
 * 				3 | 2
 */
void LEVEL_replace_tileXY(s16 x, s16 y, u8 colmap_index, u16 map_tiles[4]);

void LEVEL_update_camera(GameObject* obj);
void LEVEL_check_map_boundaries(GameObject* obj);
s16 LEVEL_get_screen_x();
s16 LEVEL_get_screen_y();


////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

inline void LEVEL_draw_collision_map() {
    VDP_setTextPlane(BG_B);

	for (u8 tile_x = 0; tile_x < SCREEN_METATILES_W; ++tile_x) {
		for (u8 tile_y = 0; tile_y < SCREEN_METATILES_H; ++tile_y) {
				
				s16 index = LEVEL_tileIDX16(tile_x, tile_y);
				if (index != 0) {
					intToStr(index, text, 2);
					VDP_drawText(text, tile_x * METATILE_W/8, tile_y * METATILE_W/8);
				} else {
					VDP_drawText("  ", tile_x * METATILE_W/8, tile_y * METATILE_W/8);
				}
		}
	}
}

void LEVEL_scroll_and_update_collision(s16 offset_x, s16 offset_y);

inline void LEVEL_draw_tile_map() {
    VDP_setTextPlane(BG_B);

	for (u8 tile_x = 0; tile_x < SCREEN_TILES_W; tile_x += 2) {
		for (u8 tile_y = 0; tile_y < SCREEN_TILES_H; tile_y += 2) {

			s16 index = LEVEL_mapbuffIDX8(tile_x, tile_y);
				if (index != 10) {
					intToStr(index, text, 1);
					VDP_drawText(text, tile_x, tile_y);
				} else {
					VDP_drawText("  ", tile_x, tile_y);
				}
		}
	}
}

// DEBUG: change for the map you want to draw
inline void LEVEL_draw_map() {
	LEVEL_draw_collision_map();
	// LEVEL_draw_tile_map();
}

void LEVEL_print_tilemap_buff();

u8 LEVEL_current_room();

#endif // _LEVEL_H_