#include "L1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <unistd.h>
#include <malloc.h>
#include <inttypes.h>
#include "sprites.h"
#include "math.h"
#include "music.h"

Thread threadHandle;
Handle threadRequest;

#define STACKSIZE (4 * 1024)

volatile bool runThread = true;

void threadMain(void *arg) {

	while(runThread) {
		svcWaitSynchronization(threadRequest, U64_MAX);
		svcClearEvent(threadRequest);
		advance();
	}
}

bool grounded = false, test = false, LRpress = false, music = true;
int offsetX = 0, offsetY = 0, i = 0, season = 2, musicdelay = 0;
float posX = 200, posY = 30, Vmomentum = 0, Hmomentum = 0, Xmomentum  = 0;

size_t spriteimg[20];
int spriteX[20];
int spriteY[20];
int scaleX[20];
int scaleY[20];
int loaded[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

size_t renderseason(char* boxtype, int seasons){
	size_t temp = sprites_devbox_idx;

	// season 1 = spring
	// season 2 = summer
	// season 3 = fall
	// season 4 = winter

	if (strcmp(boxtype, "devbox") == 0){
		if (seasons == 1) temp = sprites_devbox_idx;
		if (seasons == 2) temp = sprites_devbox2_idx;
		if (seasons == 3) temp = sprites_devbox3_idx;
		if (seasons == 4) temp = sprites_devbox4_idx;
	}
	if (strcmp(boxtype, "grass") == 0){
		if (seasons == 1) temp = sprites_grassspring_idx;
		if (seasons == 2) temp = sprites_grasssummer_idx;
		if (seasons == 3) temp = sprites_grassfall_idx;
		if (seasons == 4) temp = sprites_grasswinter_idx;
	}
	if (strcmp(boxtype, "ground") == 0){
		if (seasons == 1) temp = sprites_groundspring_idx;
		if (seasons == 2) temp = sprites_groundsummer_idx;
		if (seasons == 3) temp = sprites_groundfall_idx;
		if (seasons == 4) temp = sprites_groundwinter_idx;
	}
	if (strcmp(boxtype, "grass2x1") == 0){
		if (seasons == 1) temp = sprites_grassspring2x1_idx;
		if (seasons == 2) temp = sprites_grasssummer2x1_idx;
		if (seasons == 3) temp = sprites_grassfall2x1_idx;
		if (seasons == 4) temp = sprites_grasswinter2x1_idx;
	}
	if (strcmp(boxtype, "ground2x1") == 0){
		if (seasons == 1) temp = sprites_groundspring2x1_idx;
		if (seasons == 2) temp = sprites_groundsummer2x1_idx;
		if (seasons == 3) temp = sprites_groundfall2x1_idx;
		if (seasons == 4) temp = sprites_groundwinter2x1_idx;
	}
	return temp;
}

void actboxcoll(int result){
	if (result == 1) grounded = true, Vmomentum = 0;
	if (result == 2) posX = posX + 0.65f, Xmomentum = 0;
	if (result == 3) Vmomentum = 0;
	if (result == 4) posX = posX - 0.65f, Xmomentum = 0;
	if (result == 5) posY = posY - 5;
	if (result == 6) { grounded = true; Vmomentum = 0; posY = posY - 1; }
	if (result == 7) { grounded = true; Vmomentum = 0; posY = posY - 5; }
}

void renderbox(size_t images, int locX, int locY, int scale_X, int scale_Y, int slot, int load){
spriteimg[slot] = images;
spriteX[slot] = locX;
spriteY[slot] = locY;
scaleX[slot] = scale_X;
scaleY[slot] = scale_Y;
loaded[slot] = load;
}

void startL1(){
	//offsetY = posY;
	//offsetX = posX;
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	startSong("romfs:/arcade_thequest.mp3");

	//renderbox(renderseason("devbox", season), 50, 200, 10, 2, 1, 1);
	
	svcCreateEvent(&threadRequest,0);
	threadHandle = threadCreate(threadMain, 0, STACKSIZE, 0x3f, -2, true);


	renderbox(renderseason("devbox", season), 200, 200, 1, 1, 3, 0);
	renderbox(renderseason("devbox", season), 200, 100, 1, 1, 4, 0);

	u32 backgroundColor = C2D_Color32f(0,0,0,1);
    C2D_SpriteSheet spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");


    while (test == false){
    hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();

	if (kDown & KEY_START) break;
   	
    if (posX > 300 && Xmomentum > 0) { posX = posX - Xmomentum; offsetX = offsetX + Xmomentum;}
    if (posX < 100 && offsetX > 0 && Xmomentum < 0) { posX = posX - Xmomentum; offsetX = offsetX + Xmomentum;}

    if (grounded == true && kDown & KEY_A && season == 4) Vmomentum = Vmomentum - 2.5f;
    else if (grounded == true && kDown & KEY_A) Vmomentum = Vmomentum - 4.5f;
    else if ((kHeld & KEY_A || keysmove() == 1 || keysmove() == 2 || keysmove() == 8) && season == 3 && Vmomentum > 0)  Vmomentum = Vmomentum - 0.07f;

    if (kDown & KEY_L) season = season - 1;
    if (kDown & KEY_R) season = season + 1;
    if (season == 0) season = 4;
    if (season == 5) season = 1;

    if (kDown & KEY_Y) {
    	if (music) music = false;
    		else music = true;
    }

    if (music) svcSignalEvent(threadRequest);

    if (season != 4){

    if (keysmove() == 3 || keysmove() == 2 || keysmove() == 4) Xmomentum = Xmomentum + 0.6f, LRpress = true;
    if (keysmove() == 7 || keysmove() == 6 || keysmove() == 8) Xmomentum = Xmomentum - 0.6f, LRpress = true;

    if (LRpress == true && Xmomentum > 2.5f) Xmomentum = Xmomentum - 0.65f;
    else if (LRpress == true && Xmomentum < -2.5f) Xmomentum = Xmomentum + 0.65f;
    else if (Xmomentum > 0.7f && LRpress == false) Xmomentum = Xmomentum - 0.5f;
    else if (Xmomentum < -0.7f && LRpress == false) Xmomentum = Xmomentum + 0.5f;
    else if (Xmomentum > -0.7f && Xmomentum < 0.7f && LRpress == false) Xmomentum = 0; }

    if (season == 4){

    if (keysmove() == 3 || keysmove() == 2 || keysmove() == 4) Xmomentum = Xmomentum + 0.3f, LRpress = true;
    if (keysmove() == 7 || keysmove() == 6 || keysmove() == 8) Xmomentum = Xmomentum - 0.3f, LRpress = true;

    if (LRpress == true && Xmomentum > 4.0f) Xmomentum = Xmomentum - 0.35f;
    else if (LRpress == true && Xmomentum < -4.0f) Xmomentum = Xmomentum + 0.35f;
    else if (Xmomentum > 0.7f && LRpress == false) Xmomentum = Xmomentum - 0.5f;
    else if (Xmomentum < -0.7f && LRpress == false) Xmomentum = Xmomentum + 0.5f;
    else if (Xmomentum > -0.7f && Xmomentum < 0.7f && LRpress == false) Xmomentum = 0; }




    posX = posX + Xmomentum;
    LRpress = false;



    if (grounded == false) Vmomentum = Vmomentum + 0.15f;
    if (grounded == true && Vmomentum > 0) Vmomentum = 0;
	grounded = false;

	posY = posY + Vmomentum;

	if (posX < 0) posX = 0;

	actboxcoll(boxcoll(0 - offsetX, 190 - offsetY, 800, 50, posX, posY));
	actboxcoll(boxcoll(400 - offsetX, 165 - offsetY, 100, 25, posX, posY));
	renderbox(renderseason("grass2x1", season), 0, 190, 1, 1, 1, 1);
	renderbox(renderseason("grass2x1", season), 100, 190, 1, 1, 2, 1);
	renderbox(renderseason("grass2x1", season), 200, 190, 1, 1, 3, 1);
	renderbox(renderseason("grass2x1", season), 300, 190, 1, 1, 4, 1);
	renderbox(renderseason("grass2x1", season), 400, 190, 1, 1, 5, 1);
	renderbox(renderseason("grass2x1", season), 500, 190, 1, 1, 6, 1);
	renderbox(renderseason("grass2x1", season), 600, 190, 1, 1, 7, 1);
	renderbox(renderseason("grass2x1", season), 700, 190, 1, 1, 8, 1);
	renderbox(renderseason("ground2x1", season), 0, 215, 1, 1, 9, 1);
	renderbox(renderseason("ground2x1", season), 100, 215, 1, 1, 10, 1);
	renderbox(renderseason("ground2x1", season), 200, 215, 1, 1, 11, 1);
	renderbox(renderseason("ground2x1", season), 300, 215, 1, 1, 12, 1);
	renderbox(renderseason("ground2x1", season), 400, 215, 1, 1, 13, 1);
	renderbox(renderseason("ground2x1", season), 500, 215, 1, 1, 14, 1);
	renderbox(renderseason("ground2x1", season), 600, 215, 1, 1, 15, 1);
	renderbox(renderseason("ground2x1", season), 700, 215, 1, 1, 16, 1);
	renderbox(renderseason("grass2x1", season), 400, 165, 1, 1, 17, 1);

	//printf("test");

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_SceneBegin(top);
    C2D_TargetClear(top, backgroundColor);
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 0 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 589 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_devblock_idx), posX, posY, 0.5f, NULL, 1, 1);

    for (i = 0; i < 21; i++){
    if (loaded[i] == 1) C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, spriteimg[i]), spriteX[i] - offsetX, spriteY[i] - offsetY, 0.5f, NULL, scaleX[i], scaleY[i]);	
    }

	C3D_FrameEnd(0);
	 }
	 C2D_SpriteSheetFree(spritesheet);
	 exitSong();
	 runThread = false;
	 svcSignalEvent(threadRequest);
	 threadJoin(threadHandle, U64_MAX);
	 svcCloseHandle(threadRequest);

}