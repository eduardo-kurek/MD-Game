/**
 * Para compilar & rodar:
 * ---------------------
 *   CTRL + SHIFT + B   (gera out/rom.bin )			<< compilar
 *   [F1], Run Task, Run Gens						<< executar
 *
 * Para compilar & rodar (com extensão Genesis Code):
 * -------------------------------------------------
 *  OBS: instalar extensao Genesis Code e configurar "Gens Path"
 *
 *  Executar: $ cmd									<< pelo terminal do VSCode (extensão não funciona em PowerShell)
 *  F1 -> Genesis Code Compile Project				<< compilar
 *  F1 -> Genesis Code Compiler & Run Project		<< compilar & executar
 * 
 * LINUX
 * ==================================================================
 * Considerando que o caminho seja ~/sgdk200, para fazer build:
 * 
 * $ make GDK=~/sgdk200 -f ~/sgdk200/makefile_wine.gen
 * 
 * TILED -> customizar grade em preferências
 * GENS -> habilitar recursos avançados de debug
 *
 * PROBLEMAS
 * ------------------------------
 * > Mudar imagem de tileset fez com que o Rescomp nao encotrasse mais os tiles (problema no TMX/TSX).
 *   SOLUCAO: criar novo arquivo tsx com a mesma PNG dos tiles e usar a opção "replace tilset" do Tiled.
 * > Mensagens com KLog não aparecem no emulador
 *   SOLUCAO: Option -> Debug -> Active Development Features [x]
 * 
 * IDEIAS
 * ------------------------------
 * Bola que quebra blocos. Pode quebrar na primeia. 
 * Bola que passada através dos blocos.
 * As bolas passam pelas paredes de choque.
 * 
 * TODO
 * ------------------------------ 
 */
#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"

#include "gameobject.h"
#include "utils.h"
#include "player.h"
#include "background.h"
#include "level.h"
#include "jump_refresh.h"
#include "shoot.h"
#include "enemy.h"
#include "level.h"

// index for tiles in VRAM (first tile reserved for SGDK)
// u16 ind = 1;
u16 ind = TILE_USER_INDEX;

// glow color effect
u8 bg_colors_delay = 5;
const u16 const bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

#define MAX_OBJ 1
GameObject balls[MAX_OBJ];

typedef enum {
	START, RUNNING, END
} GAME_STATE;

GAME_STATE state = START;

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void game_init() {
	
	// init BACKGROUND, LEVEL ///////////////////////////////

	#ifdef DEBUG
	VDP_setTextPlane(BG_BACKGROUND);
	#else	
	ind += BACKGROUND_init(ind);
	#endif

	ind += LEVEL_init(ind);
	ind += SHOOT_init(ind);
	JUMPREFRESH_init();
	ENEMY_init(ind);
	
	#ifdef DEBUG
	LEVEL_draw_map();
	#endif
	
	// init GAME OBJECTS ////////////////////////////////////////////

	PLAYER_init(ind);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

static inline void color_effects() {
	--bg_colors_delay;
	if (bg_colors_delay == 0) {
		// rotate_colors_left(PAL_BACKGROUND*16, PAL_BACKGROUND*16+15);
		glow_color(PAL_BACKGROUND*16+8, bg_color_glow, 5);

		bg_colors_delay = 15;
	}
}

static inline void game_update() {
	PLAYER_update();
	JUMPREFRESH_update();
	SHOOT_update();
	ENEMY_update();

	#ifndef DEBUG
	BACKGROUND_update();
	#endif

	LEVEL_update_camera(&player);
	// color_effects();
}

////////////////////////////////////////////////////////////////////////////
// MAIN

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}
	//SYS_showFrameLoad(true);

	kprintf("Free RAM after Game Init: %d", MEM_getFree());
	
	
	SYS_doVBlankProcess();

	XGM_setLoopNumber(-1);
	XGM_startPlay(music);
		
	VDP_setScreenWidth320();
	SPR_init();
	VDP_drawImageEx(BG_BACKGROUND, &start, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
	
	while (true) {
		update_input();

		switch(state){
			case START:
				if(key_pressed(JOY_1, BUTTON_A)){
					game_init();
					state = RUNNING;
				}

				break;

			case RUNNING:
				game_update();
				u8 room = LEVEL_current_room();
				if(room == LAST_ROOM){
					SPR_reset();
					VDP_resetScreen();
					VDP_resetSprites();
					state = END;
				};
				break;

			case END:
				VDP_drawImageEx(BG_BACKGROUND, &end, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
				break;
		}

		SPR_update();
		SYS_doVBlankProcess();
	}

	return 0;
}
