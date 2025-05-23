#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <genesis.h>
#include "sprite_eng.h"
#include "globals.h"

typedef struct {
	s16 left;
	s16 right;
	s16 top;
	s16 bottom;
} BoundBox;

typedef struct {
	Sprite* sprite;
	f16 x;
	f16 y;
	f16 next_x;
	f16 next_y;
	f16 speed_x;
	f16 speed_y;
	u8 w;
	u8 h;
	BoundBox box;
	s8 w_offset;
	s8 h_offset;
	u8 anim;
	u8 health;
} GameObject;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, s8 w_offset, s8 h_offset, u8 pal, u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void GAMEOBJECT_update_boundbox(f16 x, f16 y, GameObject* obj);
void GAMEOBJECT_clamp_screen(GameObject* obj);
void GAMEOBJECT_wrap_screen(GameObject* obj);
void GAMEOBJECT_bounce_off_screen(GameObject* obj);

/**
 * Calculates the next position of a given obj using its speeds.
 * Stores the results in obj.next_[xy]
 */
void GAMEOBJECT_calculate_next_position(GameObject* obj);

/**
 * Applies the next position (obj.next_[xy]) to the real position (obj.[xy])
 */
void GAMEOBJECT_apply_next_position(GameObject* obj);

#endif // _STRUCTS_H_8