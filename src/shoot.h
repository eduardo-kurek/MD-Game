#ifndef _SHOOT_H_
#define _SHOOT_H_

#include <genesis.h>

typedef struct shoot Shoot;

u16 SHOOT_init(u16 ind);
void SHOOT_update();
void SHOOT_fire(f16 x, f16 y, s8 direction);

#endif