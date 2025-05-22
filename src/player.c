#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "level.h"
#include "utils.h"

GameObject player;
u8 jumpsRemainings = 2;

void PLAYER_input_move();
inline void PLAYER_adjust_gravity_on_ground();
inline bool PLAYER_can_jump();
void PLAYER_input_jump();
inline void PLAYER_restore_jumps_on_ground();
inline bool PLAYER_on_ceil();
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
	PLAYER_input_move();
	PLAYER_adjust_gravity_on_ground();
	PLAYER_input_jump();
	PLAYER_apply_gravity();
	PLAYER_restore_jumps_on_ground();
	GAMEOBJECT_calculate_next_position(&player);
	LEVEL_move_and_slide(&player);
	GAMEOBJECT_apply_next_position(&player);
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	PLAYER_render();
}

void PLAYER_input_move(){
	if(key_down(JOY_1, BUTTON_RIGHT))
		player.speed_x = PLAYER_SPEED;
	else if(key_down(JOY_1, BUTTON_LEFT))
		player.speed_x = -PLAYER_SPEED;
	else player.speed_x = 0;
}

void PLAYER_input_jump(){
	if(key_pressed(JOY_1, BUTTON_A) && PLAYER_can_jump()){
		player.speed_y = -PLAYER_JUMP_FORCE;
		jumpsRemainings--;
	}
}

inline void PLAYER_restore_jumps_on_ground(){
	if(PLAYER_on_ground())
		jumpsRemainings = 2;
}

/**
 * This function must be called for the gravity don't get accumulated
 * when the player is stopped on the ground and the collision still working.
 */
inline void PLAYER_adjust_gravity_on_ground(){
	if(PLAYER_on_ground())
		player.speed_y = FIX16(1);
	else if(PLAYER_on_ceil())
		player.speed_y = 0;
}

inline bool PLAYER_can_jump(){
	return jumpsRemainings > 0;
}

void PLAYER_stop_input(){
	if(key_released(JOY_1, BUTTON_RIGHT) ||
		key_released(JOY_1, BUTTON_LEFT))
		player.speed_x = 0;
}

void PLAYER_apply_gravity(){
	player.speed_y += PLAYER_GRAVITY;
	if(player.speed_y > PLAYER_MAX_GRAVITY)
		player.speed_y = PLAYER_MAX_GRAVITY;
}

void PLAYER_render(){
	SPR_setPosition(player.sprite, fix16ToInt(player.x), fix16ToInt(player.y));
	SPR_setAnim(player.sprite, player.anim);
}

bool PLAYER_on_ground(){
    return LEVEL_collision_result() & COLLISION_BOTTOM;
}

inline bool PLAYER_on_ceil(){
	return LEVEL_collision_result() & COLLISION_TOP;
}