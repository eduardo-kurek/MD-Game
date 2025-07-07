#include "enemy.h"
#include "gameobject.h"
#include "resources.h"
#include "level.h"
#include "utils.h"
#include "player.h"
#include <maths.h>

#define MAX_ENEMIES 7
#define MAX_SPEED       FIX16(8)
#define SPEED_FACTOR    FIX16(0.11)
#define FRAMES_TO_APPLY_ATRICT 2

typedef struct {
    f32 globalX;
    f32 globalY;
    u8 room;
} EnemyData;

struct enemy {
    union {
        GameObject;
        GameObject go;
    };
    u8 room;
    u16 spawnX;
    u16 spawnY;
};

Enemy enemies[MAX_ENEMIES];
Enemy* spawned_enemies[MAX_ENEMIES];
u8 num_spawned_enemies = 0;
u8 framesToApplyAtrict = 0;

inline void ENEMY_hide(Enemy* e);
inline void ENEMY_hide_all();
inline Enemy* ENEMY_next_available();
inline bool ENEMY_is_hidden(Enemy* e);
inline bool ENEMY_is_visible(Enemy* e);
inline void ENEMY_spawn_all();
inline void ENEMY_spawn(Enemy* e);
Enemy* ENEMY_from_resources(u8 index);
inline bool ENEMY_must_spawn(Enemy* e);
inline void ENEMY_apply_speed(Enemy* e);
inline void ENEMY_limit_speed(Enemy* e);
inline void ENEMY_apply_atrict(Enemy* e);
inline void ENEMY_check_wall_colision(Enemy* e);
inline void ENEMY_check_player_colision(Enemy* e);
inline void ENEMY_bounce(Enemy* e);
inline void ENEMY_render(Enemy* e);
inline bool ENEMY_is_left_to_player(Enemy* e);
inline bool ENEMY_is_right_to_player(Enemy* e);
inline bool ENEMY_is_going_left(Enemy* e);
inline bool ENEMY_is_going_right(Enemy* e);

void ENEMY_init(){
    for(u8 i = 0; i < MAX_ENEMIES; ++i){
        GAMEOBJECT_init(&enemies[i].go, &spr_enemy, 0, 0, -6, -6, PAL_PLAYER, -1);
        SPR_setPalette(enemies[i].go.sprite, PAL_PLAYER);
        SPR_setVisibility(enemies[i].sprite, HIDDEN);
        SPR_setVFlip(enemies[i].sprite, FALSE);
        enemies[i].speed_y = 0;
    }
    ENEMY_spawn_all();
}

void ENEMY_update(){
    for(u8 i = 0; i < num_spawned_enemies; ++i){
        Enemy* e = spawned_enemies[i];
        ENEMY_apply_speed(e);
        ENEMY_check_wall_colision(e);
        ENEMY_limit_speed(e);
        ENEMY_apply_atrict(e);
        ENEMY_check_player_colision(e);
        ENEMY_render(e);
    }
}

inline void ENEMY_apply_speed(Enemy* e){
    if(ENEMY_is_left_to_player(e)){
        e->speed_x += SPEED_FACTOR;
    }else if(ENEMY_is_right_to_player(e)){
        e->speed_x -= SPEED_FACTOR;
    }
}

inline void ENEMY_limit_speed(Enemy* e){
    e->speed_x = e->speed_x < -MAX_SPEED ? -MAX_SPEED : e->speed_x;
    e->speed_x = e->speed_x > MAX_SPEED ? MAX_SPEED : e->speed_x;
}

inline void ENEMY_apply_atrict(Enemy* e){
    if(framesToApplyAtrict++ < FRAMES_TO_APPLY_ATRICT) return;
    e->speed_x = fix16Mul(e->speed_x, FIX16(0.99));
}

inline void ENEMY_check_wall_colision(Enemy* e){
    s16 x, y = e->box.top + e->w/2;
    if(ENEMY_is_going_left(e))
        x = e->box.left;
    else if(ENEMY_is_going_right(e))
        x = e->box.right;
    else return;
    if(LEVEL_wallXY(x, y))
        ENEMY_bounce(e);
}

inline void ENEMY_bounce(Enemy* e){
    e->speed_x = -e->speed_x;
}

inline void ENEMY_check_player_colision(Enemy* e){
    if(PLAYER_is_colliding_with(e))
        PLAYER_respawn();
}

inline void ENEMY_render(Enemy* e){
    GAMEOBJECT_calculate_next_position(&e->go);
    GAMEOBJECT_apply_next_position(&e->go);
	GAMEOBJECT_update_boundbox(e->x, e->y, &e->go);
    s16 x = e->box.left + e->w_offset;
    s16 y = e->spawnY + e->h_offset;
    SPR_setPosition(e->sprite, x, y);
}

inline bool ENEMY_is_left_to_player(Enemy* e){ return fix16ToInt(e->x) < PLAYER_get_x(); }
inline bool ENEMY_is_right_to_player(Enemy* e){ return fix16ToInt(e->x) > PLAYER_get_x(); }
inline bool ENEMY_is_going_left(Enemy* e){ return e->speed_x < 0; }
inline bool ENEMY_is_going_right(Enemy* e){ return e->speed_x > 0; }

void ENEMY_respawn(){
    ENEMY_hide_all();
    ENEMY_spawn_all();
}

inline void ENEMY_spawn_all(){
    for(u8 i = 0; i < LEN(world1_enemies); i++){
        Enemy* e = ENEMY_from_resources(i);
        if(e == NULL) continue;
        if(ENEMY_must_spawn(e))
            ENEMY_spawn(e);
    }
}

inline void ENEMY_spawn(Enemy* e){
    SPR_setPosition(e->sprite, e->spawnX, e->spawnY);
    SPR_setVisibility(e->sprite, VISIBLE);
    spawned_enemies[num_spawned_enemies++] = e;
}

Enemy* ENEMY_from_resources(u8 index){
    Enemy* e = ENEMY_next_available();
    if(e == NULL) return NULL;
    EnemyData* data = (EnemyData*)world1_enemies[index];
    e->room = data->room;
    e->spawnX = fix32ToInt(data->globalX) % SCREEN_W;
    e->spawnY = fix32ToInt(data->globalY) % SCREEN_H - e->h_offset;
    e->speed_x = 0;
    e->x = FIX16(e->spawnX);
    e->y = FIX16(e->spawnY);
    return e;
}

inline bool ENEMY_must_spawn(Enemy* e){
    u8 room = LEVEL_current_room();
    return e->room == room;
}

Enemy* ENEMY_next_available(){
    for(u8 i = 0; i < MAX_ENEMIES; ++i)
        if(ENEMY_is_hidden(&enemies[i]))
            return &enemies[i];
    return NULL;
}

inline bool ENEMY_is_hidden(Enemy* e){ return SPR_getVisibility(e->sprite) == HIDDEN; }
inline bool ENEMY_is_visible(Enemy* e){ return SPR_getVisibility(e->sprite) == VISIBLE; }
inline void ENEMY_hide(Enemy* e){ SPR_setVisibility(e->sprite, HIDDEN); }

inline void ENEMY_hide_all(){
    for(u8 i = 0; i < MAX_ENEMIES; ++i)
        ENEMY_hide(&enemies[i]);
    num_spawned_enemies = 0;
}