#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "level.h"
#include "utils.h"

#define SPAWN_X 64
#define SPAWN_Y 192

GameObject player;
u8 jumpsRemainings = 3;
f16 checkpoint_x = FIX16(SPAWN_X);
f16 checkpoint_y = FIX16(SPAWN_Y);
s16 checkpoint_screen_x = 0;
s16 checkpoint_screen_y = 0;

inline void PLAYER_input_move();
inline void PLAYER_adjust_gravity_on_ground();
inline bool PLAYER_can_jump();
inline void PLAYER_input_jump();
inline void PLAYER_input_checkpoint();
inline void PLAYER_input_restart();
inline void PLAYER_jump_release();
inline void PLAYER_restore_jumps_on_ground();
inline bool PLAYER_on_ceil();
inline void PLAYER_stop_input();
inline void PLAYER_apply_gravity();
inline void PLAYER_render();
inline bool PLAYER_is_jumping();
inline void PLAYER_check_thorn();

////////////////////////////////////////////////////////////////////////////
// INIT

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_player, SPAWN_X, SPAWN_Y, -4, 0, PAL_PLAYER, ind);
	player.health = PLAYER_MAX_HEALTH;
	return ind;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void PLAYER_update() {
	PLAYER_input_move();
	PLAYER_adjust_gravity_on_ground();
	PLAYER_input_jump();
	PLAYER_jump_release();
	PLAYER_apply_gravity();
	PLAYER_restore_jumps_on_ground();
	GAMEOBJECT_calculate_next_position(&player);
	LEVEL_move_and_slide(&player);
	GAMEOBJECT_apply_next_position(&player);
	PLAYER_input_checkpoint();
	PLAYER_input_restart();
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	PLAYER_check_thorn();
	PLAYER_render();
}

inline void PLAYER_input_move(){
	if(key_down(JOY_1, BUTTON_RIGHT)){
		player.speed_x = PLAYER_SPEED;
		player.anim = 1;
	}
	else if(key_down(JOY_1, BUTTON_LEFT)){
		player.speed_x = -PLAYER_SPEED;
		player.anim = 1;
	}
	else{
		player.speed_x = 0;
		player.anim = 0;
	}
}

inline void PLAYER_input_jump(){
	if(key_pressed(JOY_1, BUTTON_A) && PLAYER_can_jump()){
		player.speed_y = -PLAYER_JUMP_FORCE;
		jumpsRemainings--;
	}
}

inline void PLAYER_input_checkpoint(){
	if(key_pressed(JOY_1, BUTTON_C))
		PLAYER_update_checkpoint();
}

inline void PLAYER_input_restart(){
	if(key_pressed(JOY_1, BUTTON_B))
		PLAYER_respawn();
}

inline void PLAYER_jump_release(){
	if(key_released(JOY_1, BUTTON_A) && PLAYER_is_jumping()){
		player.speed_y = FIX16(0);
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
	return jumpsRemainings > 1;
}

inline void PLAYER_stop_input(){
	if(key_released(JOY_1, BUTTON_RIGHT) ||
		key_released(JOY_1, BUTTON_LEFT))
		player.speed_x = 0;
}

inline void PLAYER_apply_gravity(){
	player.speed_y += PLAYER_GRAVITY;
	if(player.speed_y > PLAYER_MAX_GRAVITY)
		player.speed_y = PLAYER_MAX_GRAVITY;
}

inline void PLAYER_render(){
	s16 x = player.box.left + player.w_offset;
	s16 y = player.box.top + player.h_offset;
	SPR_setPosition(player.sprite, x, y);
	SPR_setAnim(player.sprite, player.anim);
}

inline bool PLAYER_is_jumping(){
	return player.speed_y < 0;
}

inline void PLAYER_check_thorn(){
	u8 id = LEVEL_tileXY(player.box.left + player.w/2, player.box.top + player.h/2);
	if(IDX_IS_THORN(id))
		PLAYER_respawn();
}

inline bool PLAYER_on_ground(){
    return LEVEL_collision_result() & COLLISION_BOTTOM;
}

void PLAYER_respawn(){
	s16 offset_x = checkpoint_screen_x - LEVEL_get_screen_x();
	s16 offset_y = checkpoint_screen_y - LEVEL_get_screen_y();
	LEVEL_scroll_and_update_collision(offset_x, offset_y);
	player.x = checkpoint_x;
	player.y = checkpoint_y;
	// Na verdade ele vai ter um pulo apenas, devo colocar
	// um a mais para funcionar, por algum motivo :)
	jumpsRemainings = 2;

	#ifdef DEBUG

	kprintf("Respawning: player(%d, %d). screen offset(%d, %d)", 
		fix16ToInt(checkpoint_x), fix16ToInt(checkpoint_y), offset_x, offset_y);
	#endif
}

void PLAYER_update_checkpoint(){
	checkpoint_x = player.x;
	checkpoint_y = player.y;
	checkpoint_screen_x = LEVEL_get_screen_x();
	checkpoint_screen_y = LEVEL_get_screen_y();

	#ifdef DEBUG
	kprintf("Updating checkpoint: player(%d, %d). screen(%u, %u)", 
		fix16ToInt(checkpoint_x), fix16ToInt(checkpoint_y), checkpoint_screen_x, checkpoint_screen_y);
	#endif
}

inline bool PLAYER_on_ceil(){
	return LEVEL_collision_result() & COLLISION_TOP;
}