#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "L1.h"
#include <citro3d.h>
#include <citro2d.h>
#include <unistd.h>
#include "sprites.h"
#include "mainmenu.h"
#include "math.h"
#include "thread.h"

bool makeselection = false, cancelselection = false, inLSelect = false;
int backoffset = 0, selection = 1, seasontimer = 0, scrolloffset = 0, arrowYoffset = 0, temp = 0;
size_t sprite_ground = sprites_devbox_idx;
size_t sprite_grass = sprites_devbox_idx;


int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    MakeThread();
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    u32 backgroundColor = C2D_Color32f(0,0,0,1);
    romfsInit();
    C2D_SpriteSheet spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    C2D_SpriteSheet mainmenu = C2D_SpriteSheetLoad("romfs:/gfx/mainmenu.t3x");
    romfsExit();


	while (aptMainLoop())
	{
		scrolloffset = scrolloffset + 1;
    	seasontimer = seasontimer + 1;
    	backoffset = backoffset + 1;

    	hidScanInput();
		u32 kDown = hidKeysDown();
		circlePosition pos;
		hidCircleRead(&pos);

    	if (kDown & KEY_A) makeselection = true;
		if (kDown & KEY_B) cancelselection = true;
		if (pos.dy > 80 || kDown & KEY_DUP) selection = selection - 1;
		if (pos.dy < -80 || kDown & KEY_DDOWN) selection = selection + 1;
		if (selection == 0 && inLSelect == false) selection = 1;
		if (selection == 5 && inLSelect == false) selection = 4;
		if (selection == 0 && inLSelect == true) selection = 1;
		if (selection == 7 && inLSelect == true) selection = 6;

		if (makeselection == true && selection == 1 && inLSelect == false) {
			makeselection = false;
			inLSelect = true;
		}

		if (cancelselection == true && inLSelect == true){
			cancelselection = false;
			inLSelect = false;
			selection = 1;
		}

		if (makeselection == true && inLSelect == true){
			if (selection == 1) {
			consoleInit(GFX_BOTTOM, NULL);
			startL1(); }
			if (selection == 2) {
			consoleInit(GFX_BOTTOM, NULL);
			printf("L2 Not implemented yet. Go yell at meme or something"); }
			if (selection == 3) {
			consoleInit(GFX_BOTTOM, NULL);
			printf("L3 Not implemented yet. Go yell at meme or something"); }
			if (selection == 4) {
			consoleInit(GFX_BOTTOM, NULL);
			printf("L4 Not implemented yet. Go yell at meme or something"); }
			if (selection == 5) {
			consoleInit(GFX_BOTTOM, NULL);
			printf("L5 Not implemented yet. Go yell at meme or something"); }
			if (selection == 6) {
			consoleInit(GFX_BOTTOM, NULL);
			printf("L6 Not implemented yet. Go yell at meme or something"); }
		}

		if (makeselection == true && inLSelect == false){
			if (selection == 1) {
			makeselection = false;
			inLSelect = true; }
		
			if (selection == 2) printf("\x1b[1;1HSettings Not implemented yet. Go yell at meme or something");
			if (selection == 3) printf("\x1b[1;1HCredits Not implemented yet. Go yell at meme or something ");

			if (selection == 4) break;
		}

		if (inLSelect == false){
		if (selection == 1) arrowYoffset = 28;
		if (selection == 2) arrowYoffset = 42;
		if (selection == 3) arrowYoffset = 69;
		if (selection == 4) arrowYoffset = 89; }

		if (inLSelect == true){
		if (selection == 1) arrowYoffset = 0;
		if (selection == 2) arrowYoffset = 13;
		if (selection == 3) arrowYoffset = 26;
		if (selection == 4) arrowYoffset = 39;
		if (selection == 5) arrowYoffset = 52;
		if (selection == 6) arrowYoffset = 65;
		}

		if (makeselection) makeselection = false;
		if (cancelselection) cancelselection = false;


    	if (seasontimer > 0 && seasontimer < 200) { sprite_grass = sprites_grasssummer2x1_idx; sprite_ground = sprites_groundsummer2x1_idx; }
    	if (seasontimer > 200 && seasontimer < 400) { sprite_grass = sprites_grassfall2x1_idx; sprite_ground = sprites_groundfall2x1_idx; }
    	if (seasontimer > 400 && seasontimer < 600) { sprite_grass = sprites_grasswinter2x1_idx; sprite_ground = sprites_groundwinter2x1_idx; }
    	if (seasontimer > 600 && seasontimer < 800) { sprite_grass = sprites_grassspring2x1_idx; sprite_ground = sprites_groundspring2x1_idx; }
    	if (seasontimer > 800) seasontimer = 0;
    	if (scrolloffset == 100) scrolloffset = 0;
    	if (backoffset == 1767) backoffset = 0;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    	C2D_SceneBegin(top);
    	C2D_TargetClear(top, backgroundColor);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 0 - (backoffset / 3), 0, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 589 - (backoffset / 3), 0, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_devblock_idx), 250, 152, 0.5f, NULL, 1, 2);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_grass), 0 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_grass), 100 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_grass), 200 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_grass), 300 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_grass), 400 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_ground), 0 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_ground), 100 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_ground), 200 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_ground), 300 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprite_ground), 400 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(mainmenu, mainmenu_fade_idx), 0, 0, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(mainmenu, mainmenu_title_idx), 20, 25, 0.5f, NULL, 1, 1);
    	//85, 28

    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(mainmenu, mainmenu_main_idx), 20, 45, 0.5f, NULL, 1, 1);
    	if (inLSelect == false) C2D_DrawImageAt(C2D_SpriteSheetGetImage(mainmenu, mainmenu_arrow_idx), 105, 45 + arrowYoffset, 0.5f, NULL, 1, 1);
    	if (inLSelect == true){
    		C2D_DrawImageAt(C2D_SpriteSheetGetImage(mainmenu, mainmenu_arrow_idx), 105, 73, 0.5f, NULL, 1, 1);
    		C2D_DrawImageAt(C2D_SpriteSheetGetImage(mainmenu, mainmenu_arrow_idx), 185, 73 + arrowYoffset, 0.5f, NULL, 1, 1);
    		C2D_DrawImageAt(C2D_SpriteSheetGetImage(mainmenu, mainmenu_levels_idx), 120, 73, 0.5f, NULL, 1, 1);
    	}

    	C3D_FrameEnd(0); 

	}
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	ExitThread();
	gfxExit();
	return 0;
}