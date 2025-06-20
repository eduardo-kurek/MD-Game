#include "shoot.h"
#include "gameobject.h"
#include "resources.h"
#include "globals.h"
#include "level.h"
#include "player.h"

#define MAX_SHOOTS 5
#define SHOOT_SPEED 3

struct shoot {
    union {
        GameObject;
        GameObject go;
    };
};

Shoot shoots[MAX_SHOOTS];
Shoot* currentShoot;

inline Shoot* SHOOT_next_available();
inline void SHOOT_do_update(Shoot* s);
inline bool SHOOT_is_visible(Shoot* s);
inline bool SHOOT_is_hidden(Shoot* s);
inline void SHOOT_update_position(Shoot* s);
inline bool SHOOT_collided(Shoot* s);
inline bool SHOOT_is_out_of_screen(Shoot* s);
inline void SHOOT_hide(Shoot* s);
inline bool SHOOT_is_going_left(Shoot* s);
inline bool SHOOT_hitted_wall(Shoot* s);
inline bool SHOOT_hitted_left_wall(Shoot* s);
inline bool SHOOT_hitted_right_wall(Shoot* s);
inline bool SHOOT_hitted_checkpoint(Shoot* s);

u16 SHOOT_init(u16 ind){
    for(u8 i = 0; i < MAX_SHOOTS; ++i){
        GAMEOBJECT_init(&shoots[i].go, &spr_shoot, 0, 0, -8, -6, PAL_PLAYER, -1);
        shoots[i].speed_y = 0;
        SPR_setVisibility(shoots[i].sprite, HIDDEN);
    }
    return ind;
}

void SHOOT_update(){
    for(u8 i = 0; i < MAX_SHOOTS; ++i){
        Shoot* s = &shoots[i];
        if(SHOOT_is_hidden(s)) continue;
        SHOOT_do_update(s);
    }
}

inline void SHOOT_do_update(Shoot* s){
    SHOOT_update_position(s);
    if(SHOOT_collided(s))
        SHOOT_hide(s);
}

inline void SHOOT_update_position(Shoot* s){
    s->x += s->speed_x;
    GAMEOBJECT_update_boundbox(s->x, s->y, &s->go);
    s16 x = s->box.left + s->w_offset;
    s16 y = s->box.top + s->h_offset;
    SPR_setPosition(s->sprite, x, y);
}

inline bool SHOOT_collided(Shoot* s){
    if(SHOOT_hitted_checkpoint(s)){
        PLAYER_update_checkpoint();
        return true;
    }

    return SHOOT_is_out_of_screen(s)
        || SHOOT_hitted_wall(s);
}

inline bool SHOOT_is_out_of_screen(Shoot* s){
    s16 x = s->box.left + s->w_offset;
    if(x < -s->w || x > SCREEN_W)
        return true;
    return false;
}

inline bool SHOOT_hitted_wall(Shoot* s){
    if(SHOOT_is_going_left(s))
        return SHOOT_hitted_left_wall(s);
    return SHOOT_hitted_right_wall(s);
}

inline bool SHOOT_is_going_left(Shoot* s){ return s->speed_x < 0; }

inline bool SHOOT_hitted_left_wall(Shoot* s){
    s16 x = s->box.left;
    s16 y = s->box.top + s->h_offset;
    return LEVEL_wallXY(x, y) || LEVEL_wallXY(x, y + s->h);
}

inline bool SHOOT_hitted_right_wall(Shoot* s){
    s16 x = s->box.left + s->w_offset;
    s16 y = s->box.top + s->h_offset;
    return LEVEL_wallXY(x + s->w, y) || LEVEL_wallXY(x + s->w, y + s->h);
}

inline bool SHOOT_hitted_checkpoint(Shoot* s){
    s16 x = s->box.left + s->w/2;
	s16 y = s->box.top + s->h/2;
	u8 id = LEVEL_tileXY(x, y);
	if(IDX_IS_CHECKPOINT(id))
		return true;
    return false;
}


void SHOOT_fire(f16 x, f16 y, s8 direction){
    Shoot* shoot = SHOOT_next_available();
    if(shoot == NULL) return;
    shoot->x = x;
    shoot->y = y;
    shoot->speed_x = FIX16(SHOOT_SPEED * direction);
    SPR_setVisibility(shoot->sprite, VISIBLE);
}

void SHOOT_hide_all(){
    for(u8 i = 0; i < MAX_SHOOTS; ++i)
        SHOOT_hide(&shoots[i]);
}

inline Shoot* SHOOT_next_available(){
    for(u8 i = 0; i < MAX_SHOOTS; ++i){
        if(SHOOT_is_hidden(&shoots[i])){
            return &shoots[i];
        }
    }
    return NULL;
}

inline bool SHOOT_is_visible(Shoot *shoot){ return SPR_getVisibility(shoot->sprite) == VISIBLE; } 
inline bool SHOOT_is_hidden(Shoot *shoot){  return SPR_getVisibility(shoot->sprite) == HIDDEN; }
inline void SHOOT_hide(Shoot* s){ SPR_setVisibility(s->sprite, HIDDEN); }
