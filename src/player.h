#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "gameobject.h"
#include "resources.h"

#define ANIM_VERTICAL    0
#define ANIM_HORIZONTAL  1

// #define PLAYER_SPEED45 FIX16(0.707)

extern GameObject player;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init(u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_update();
inline bool PLAYER_on_ground();

#endif // _PLAYER_H_