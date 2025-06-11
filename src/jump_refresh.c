#include "jump_refresh.h"
#include "level.h"

#define MAX_JUMPREFRESHS 10

#define FRAMES_TO_RESPAWN 75 // 2.5 seconds
#define AVAILABLE -1

typedef struct jump_refresh{
    s16 x;
    s16 y;
    s16 remainingFrames;
} JumpRefresh;

JumpRefresh refreshs[MAX_JUMPREFRESHS];

u8 JUMPREFRESH_get_available_index(){
    for(u8 i = 0; i < MAX_JUMPREFRESHS; ++i)
        if(refreshs[i].remainingFrames == AVAILABLE)
            return i;
    return -1;
}


void JUMPREFRESH_remove(s16 x, s16 y){
    u8 index = JUMPREFRESH_get_available_index();
    refreshs[index].x = x;
    refreshs[index].y = y;
    refreshs[index].remainingFrames = FRAMES_TO_RESPAWN;
    LEVEL_remove_tileXY(x, y);
}

inline void JUMPREFRESH_decrement(JumpRefresh* jr){
    if(jr->remainingFrames > 0)
        jr->remainingFrames--;
}

inline bool JUMPREFRESH_must_respawn(JumpRefresh* jr){
    return jr->remainingFrames == 0;
}

inline void JUMPREFRESH_respawn(JumpRefresh* jr){
    LEVEL_replace_tileXY(jr->x, jr->y, IDX_JUMP_REFRESH, (u16[]){IDX_JUMP_REFRESH, IDX_JUMP_REFRESH+1, IDX_JUMP_REFRESH+14, IDX_JUMP_REFRESH+13});
    jr->remainingFrames = AVAILABLE;
}

void JUMPREFRESH_update(){
    for(u8 i = 0; i < MAX_JUMPREFRESHS; ++i){
        JumpRefresh* j = &refreshs[i];
        JUMPREFRESH_decrement(j);
        if(JUMPREFRESH_must_respawn(j))
            JUMPREFRESH_respawn(j);
    }
}

void JUMPREFRESH_init(){
    for(u8 i = 0; i < MAX_JUMPREFRESHS; ++i){
        refreshs[i].remainingFrames = AVAILABLE;
    }
}