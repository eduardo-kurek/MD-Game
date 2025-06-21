#ifndef _JUMP_REFRESH_H_
#define _JUMP_REFRESH_H_

#include <genesis.h>
#include "globals.h"

void JUMPREFRESH_remove(s16 x, s16 y);
void JUMPREFRESH_update();
void JUMPREFRESH_init();
void JUMPREFRESH_restore_all();

#endif