#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "loadlevel.h"
#include <citro3d.h>
#include <citro2d.h>
#include <unistd.h>
#include "sprites.h"
#include "mainmenu.h"
#include "math.h"
#include "thread.h"
#include "background.h"
#include "music.h"
#include "textures.h"
#include "graphics.h"

bool makeselection = false, cancelselection = false, inLSelect = false, settings = false, firstboot = true;
int musicT = 1, debugT = 0;
int backoffset = 0, selection = 1, seasontimer = 0, scrolloffset = 0, arrowYoffset = 0;
int seasonmenu = 1;

void callset(){
	settingsaj(debugT, musicT);
}

int main(int argc, char **argv)
{
	MakeThread();
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    Init_Text();
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	u32 backgroundColor = C2D_Color32f(0,0,0,1);
	consoleInit(GFX_BOTTOM, NULL);
	printf("\nLoading Textures! Please wait...");
    texload();
    consoleInit(GFX_BOTTOM, NULL);



    makeselection = false;



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
		if (selection == 1 && settings == true) selection = 2;
		if (selection == 4 && settings == true) selection = 3; 

		if (makeselection == true && selection == 1 && inLSelect == false) {
			makeselection = false;
			inLSelect = true;
		}

		if (cancelselection == true && inLSelect == true){
			cancelselection = false;
			inLSelect = false;
			selection = 1;
		}

		if (cancelselection == true && settings == true){
			cancelselection = false;
			settings = false;
			selection = 2;
		}

		if (makeselection == true && inLSelect == true){
			if (selection == 1) {
			makeselection = false;
			callset();
			consoleInit(GFX_BOTTOM, NULL);
			loadlevel("romfs:/test.txt");
			consoleInit(GFX_BOTTOM, NULL);
			}
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

		if (makeselection == true && settings == true){
			if (selection == 2 && debugT == 1) debugT = 0;
			else if (selection == 2 && debugT == 0) debugT = 1;
			if (selection == 3 && musicT == 1) musicT = 0;
			else if (selection == 3 && musicT == 0) musicT = 1; 
		}


		if (makeselection == true && inLSelect == false && settings == false){
			if (selection == 1) {
			makeselection = false;
			inLSelect = true; }
		
			if (selection == 2) settings = true;
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

		if (settings == true){
		if (selection == 2) arrowYoffset = 0;
		if (selection == 3) arrowYoffset = 16;
		}

		if (makeselection) makeselection = false;
		if (cancelselection) cancelselection = false;


    	if (seasontimer > 0 && seasontimer < 200) seasonmenu = 1;
    	if (seasontimer > 200 && seasontimer < 400) seasonmenu = 2;
    	if (seasontimer > 400 && seasontimer < 600) seasonmenu = 3;
    	if (seasontimer > 600 && seasontimer < 800) seasonmenu = 4;
    	if (seasontimer > 800) seasontimer = 0;
    	if (scrolloffset == 100) scrolloffset = 0;
    	if (backoffset == 1767) backoffset = 0;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    	C2D_SceneBegin(top);
    	C2D_TargetClear(top, backgroundColor);
    	C2D_DrawImageAt(backsmw, 0 - (backoffset / 3), 0, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(backsmw, 589 - (backoffset / 3), 0, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(player, 250, 152, 0.5f, NULL, 1, 2);
    	C2D_DrawImageAt(getsprite(3, seasonmenu, 0), 0 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(3, seasonmenu, 0), 100 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(3, seasonmenu, 0), 200 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(3, seasonmenu, 0), 300 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(3, seasonmenu, 0), 400 - scrolloffset, 190, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(4, seasonmenu, 0), 0 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(4, seasonmenu, 0), 100 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(4, seasonmenu, 0), 200 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(4, seasonmenu, 0), 300 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(getsprite(4, seasonmenu, 0), 400 - scrolloffset, 215, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(fadescr, 0, 0, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(titlemenu, 20, 20, 0.5f, NULL, 1, 1);
    	//85, 28



    	C2D_DrawImageAt(menutxt, 20, 45, 0.5f, NULL, 1, 1);
    	if (inLSelect == false && settings == false) C2D_DrawImageAt(menuarr, 105, 45 + arrowYoffset, 0.5f, NULL, 1, 1);
    	if (inLSelect == true){
    		C2D_DrawImageAt(menuarr, 105, 73, 0.5f, NULL, 1, 1);
    		C2D_DrawImageAt(menuarr, 185, 73 + arrowYoffset, 0.5f, NULL, 1, 1);
    		C2D_DrawImageAt(menulvltxt, 120, 73, 0.5f, NULL, 1, 1);
    	}

    	if (settings == true){
    		C2D_DrawImageAt(menuarr, 105, 87, 0.5f, NULL, 1, 1);
    		C2D_DrawImageAt(menuarr, 168, 87 + arrowYoffset, 0.5f, NULL, 1, 1);
    		C2D_DrawImageAt(menuset, 120, 87, 0.5f, NULL, 1, 1);
    		if (debugT == true) C2D_DrawImageAt(menuon, 180, 87, 0.5f, NULL, 1, 1);
    		if (debugT == false) C2D_DrawImageAt(menuoff, 180, 87, 0.5f, NULL, 1, 1);
    		if (musicT == true) C2D_DrawImageAt(menuon, 180, 103, 0.5f, NULL, 1, 1);
    		if (musicT == false) C2D_DrawImageAt(menuoff, 180, 103, 0.5f, NULL, 1, 1);
    	}

    	C3D_FrameEnd(0); 

	}
	texfree();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	ExitThread();
	exitSong();
	romfsExit();
	return 0;
}