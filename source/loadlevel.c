#include "loadlevel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <unistd.h>
#include <malloc.h>
#include <inttypes.h>
#include "math.h"
#include "music.h"
#include "thread.h"
#include "textures.h"
#include "graphics.h"

C3D_RenderTarget* top = NULL;
u32 backgroundColor = 0;

bool debug = false, music = true, pausegame = false, dead = false, quit = true;
float offsetX = 0, offsetY = 0;
float posY = 150, posX = 50;
int season = 1;
int linesinlevel = 0;
int i = 0, temp = 0;
int temphorz, tempvert, aapress, run = 0;
int coins = 0;
C2D_Image backgroundid;
char* seasontext = "";
char hudtext[100];

void settingsaj(int optiondebug, int optionmusic){
	if (optiondebug == 1) debug = true;
	else debug = false;
	if (optionmusic == 1) music = true;
	else music = false;
}

void Ajustoffset(char* vartoajust, float offsetofvar){
	if (strcmp(vartoajust, "X") == 0) offsetX = offsetX + offsetofvar;
	if (strcmp(vartoajust, "Y") == 0) offsetY = offsetY + offsetofvar;
	if (strcmp(vartoajust, "pY") == 0) posY = posY + offsetofvar;
	if (strcmp(vartoajust, "pX") == 0) posX = posX + offsetofvar;
}


//https://github.com/joel16/3DShell/blob/master/source/textures.c

void loadlevel(const char* level){
	consoleInit(GFX_BOTTOM, NULL);
	printf("Loading music...\n");
	FILE* file = fopen (level, "r");
	if (file == NULL) svcBreak(USERBREAK_PANIC);
	fscanf(file, "%d", &temp);
	startSong(musicget(temp));
	fscanf(file, "%d", &temp);
	backgroundid = backgroundimg(temp);
	
	while (!feof (file)){
		fscanf(file, "%d", &temp); 
		i++;
		//printf("%d %d\n", i, temp);
	}

	linesinlevel = i / 8;
	i = 0;
	rewind(file);

	int texture[linesinlevel];
	int type[linesinlevel];
	int locX[linesinlevel];
	int locY[linesinlevel];
	int Optional1[linesinlevel];
	int Optional2[linesinlevel];
	int Optional3[linesinlevel];
	int Optional4[linesinlevel];

	printf("Loading level...\n");
	fscanf(file, "%d", &temp);
	fscanf(file, "%d", &temp);
	while (!feof (file)){
		fscanf(file, "%d %d %d %d %d %d %d %d", &type[i], &texture[i], &locX[i], &locY[i], &Optional1[i], &Optional2[i], &Optional3[i], &Optional4[i]); 
		if (debug) printf("%d %d %d %d %d %d %d %d\n", type[i], texture[i], locX[i], locY[i], Optional1[i], Optional2[i], Optional3[i], Optional4[i]);	
		i++;
	}

	fclose(file);

	begin:
	pausegame = false, dead = false, quit = false;
	offsetX = 0, offsetY = 0;
	posY = 100, posX = 50;
	season = 1;
	run = 0;
	coins = 0;

	while(1){
		hidScanInput();
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
	
		if (kDown & KEY_START) pausegame = true;
		if (kDown & KEY_L) season = season - 1;
   		if (kDown & KEY_R) season = season + 1;
   		if (season == 0) season = 4;
   		if (season == 5) season = 1;

   		if (music == true) RunThread();

   		if (kDown & KEY_X && debug == true){
   			i = 0;
   			while (i < linesinlevel){
   				printf("%d %d %d %d %d %d %d %d\n", type[i], texture[i], locX[i], locY[i], Optional1[i], Optional2[i], Optional3[i], Optional4[i]);
   				i++;
   			}

   		}

   		posX = posX + calcHmomentum(posX, offsetX, season);

   		i = 0;
   		while(i < linesinlevel){
   		
   			if (type[i] == 3){
   				ladder(locX[i] - offsetX, locY[i] - offsetY, Optional1[i], Optional2[i], posX, posY, offsetY);
   			}	

   			if (type[i] == 2){
   				boxcoll(locX[i] - offsetX, locY[i] - offsetY, Optional1[i], Optional2[i], posX, posY);
   			}

   			if (type[i] == 4 && season == 4){
   				boxcoll(locX[i] - offsetX, locY[i] - offsetY, Optional1[i], Optional2[i], posX, posY);
   			}

   			if (type[i] == 10){
   				if (IsInsideBox(locX[i] - offsetX, locY[i] - offsetY, 20, 20, posX, posY) == 1){
   					type[i] = -1;
   					type[i + 1] = -1;
   					coins++;
   				}
   			}

   			if (type[i] == 30){
   				if (IsInsideBox(locX[i] - offsetX, locY[i] - offsetY, Optional1[i], Optional2[i], posX, posY) == 1){
   					run = 1;
   					type[i] = -1;
   					type[i+1] = -1; 
   				}
   			}

   			if (type[i] == 31 && run == 1){
   				if (type[i+1] == -1) type[i+1] = 1;
   				if (IsInsideBox(locX[i] - offsetX, locY[i] - offsetY, Optional1[i], Optional2[i], posX, posY) == 1) quit = true;
   			}
   			i++;
   		}

   		if (kDown & KEY_A) aapress = 1;
		posY = posY + calcVmomentum(posY, offsetY, season, aapress);
		aapress = 0;
		if (posY > 400) dead = true;

		tempvert = offsetY;
   		temphorz = offsetX;

   		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    	C2D_SceneBegin(top);
    	C2D_TargetClear(top, backgroundColor);

    	C2D_DrawImageAt(backgroundid, 0 - temphorz, 0, 0.5f, NULL, 1, 1);

    	C2D_DrawImageAt(player, posX, posY, 0.5f, NULL, 1, 2);

    	i = 0;
		while(i < linesinlevel){

			if (type[i] == 1){
				if (locX[i] - offsetX < 500 && locX[i] - offsetX > -100){
				C2D_DrawImageAt(getsprite(texture[i], season, run), locX[i] - offsetX, locY[i] - offsetY, 0.5f, NULL, Optional1[i], Optional2[i]);
				} 
			}
			i++;
		}

		if (season == 1) seasontext = "Spring";
		if (season == 2) seasontext = "Summer";
		if (season == 3) seasontext = "Fall";
		if (season == 4) seasontext = "Winter";

		snprintf(hudtext, 100, "%s\n    X %d", seasontext, coins);

		C2D_DrawImageAt(getsprite(10, season, run), -20, 0, 0.5f, NULL, 1, 5);
		C2D_DrawImageAt(getsprite(9, season, run), 1, 31, 0.5f, NULL, 1, 1);

		Draw_Text(0, 0, 0.9f, hudtext);

		C3D_FrameEnd(0);

		if (quit == true) break;

		if (kDown & KEY_START) break;
	}
}

