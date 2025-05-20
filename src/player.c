#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "level.h"
#include "utils.h"

GameObject player;

void PLAYER_get_input();
void PLAYER_stop_input();
void PLAYER_apply_gravity();
void PLAYER_render();

////////////////////////////////////////////////////////////////////////////
// INIT

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_ship, SCREEN_W/2-12, SCREEN_H/2-12, PAL_PLAYER, ind);
	player.health = PLAYER_MAX_HEALTH;
	return ind;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void PLAYER_update() {
	PLAYER_get_input();
	PLAYER_apply_gravity();
	GAMEOBJECT_calculate_next_position(&player);
	LEVEL_move_and_slide(&player);
	GAMEOBJECT_apply_next_position(&player);
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	PLAYER_render();
}

void PLAYER_get_input(){
	if(key_down(JOY_1, BUTTON_RIGHT))
		player.speed_x = PLAYER_SPEED;
	else if(key_down(JOY_1, BUTTON_LEFT))
		player.speed_x = -PLAYER_SPEED;
	else player.speed_x = 0;
}

void PLAYER_stop_input(){
	if(key_released(JOY_1, BUTTON_RIGHT) ||
		key_released(JOY_1, BUTTON_LEFT))
		player.speed_x = 0;
}

void PLAYER_apply_gravity(){
	player.speed_y -= PLAYER_GRAVITY;
	if(player.speed_y < PLAYER_MAX_GRAVITY)
		player.speed_y = PLAYER_MAX_GRAVITY;
}

void PLAYER_render(){
	SPR_setPosition(player.sprite, fix16ToInt(player.x), fix16ToInt(player.y));
	SPR_setAnim(player.sprite, player.anim);
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Always moving (doesn't stop when dpad released)
 * - Four directions, mutually exclusive
 */
void PLAYER_get_input_dir4() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 0;
	}
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 4;
	}
	else
	if (key_down(JOY_1, BUTTON_UP)) {
		player.speed_x = 0;
		player.speed_y = -PLAYER_SPEED;
		player.anim = 2;
	}
	else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.speed_x = 0;
		player.speed_y = PLAYER_SPEED;
		player.anim = 6;
	} 
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Stop when dpad released
 * - Eight directions + fix for diagonals
 */
