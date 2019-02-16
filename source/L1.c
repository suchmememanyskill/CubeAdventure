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
#include "pause.h"
#include "math.h"
#include "music.h"
#include "thread.h"
#include "text.h"

//Define variables ------------------------------------------------------------

bool grounded = false, test = false, LRpress = false, music = true, debug = false, flipdebug = false, pausegame = false, death = false, quit = false, yeet = false, cameratrigger1 = true;
int i = 0, season = 2, debugdelay = 0, pauseselection = 1, arrowoffset = 0, watertimer = 0, tempvert = 0, temphorz = 0, cameratriggeroffset1 = 0;
float posX = 200, posY = 150, Vmomentum = 0, Hmomentum = 0, Xmomentum  = 0, offsetY = 0, offsetX = 0, yeettimer = 30;

size_t spriteimg[40];
int spriteX[40];
int spriteY[40];
int scaleX[40];
int scaleY[40];
int loaded[40] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int rotation[40];
int didvaluechange[40] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//Move, Ladder, season, season2, wintertext, springtext, falltext, escapeblocktext (inactive), escapetext.
int textboxX[10] = {100, 850, 1000, 1150, 1500, 1950, 2400, -400, 2600};
int textboxY[10] = {25, 40, -100, -100, 40, 40, 50, 50, 40};
size_t textboxtext[10] = {text_move_idx, text_laddertext_idx, text_seasonstext_idx, text_seasons2text_idx, text_wintertext_idx, text_springtext_idx, text_falltext_idx, text_escapeblocktext_idx, text_escapetext_idx};

//Take settings from main menu ----------------------------------------

void settingsL1(int Set){
	if (Set == 1) debug = false;
	if (Set == 2) debug = true;
	if (Set == 3) music = false;
	if (Set == 4) music = true;
}

//Define texture use ---------------------------------------------------

size_t getspriteseason(char* boxtype, int seasons){
	size_t temp = sprites_devbox_idx;

	// season 1 = spring > jump boost, slower movement
	// season 2 = summer > nothing
	// season 3 = fall > gliding, slightly slower movement
	// season 4 = winter > faster, needs slight jump buff

	if (strcmp(boxtype, "devbox") == 0){
		if (yeet){
		if (seasons == 1) temp = sprites_devbox_idx;
		if (seasons == 2) temp = sprites_devbox2_idx;
		if (seasons == 3) temp = sprites_devbox3_idx;
		if (seasons == 4) temp = sprites_devbox4_idx; }
		if (yeet == false) temp = sprites_devboxmissing_idx;
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
	if (strcmp(boxtype, "grassslope") == 0){
		if (seasons == 1) temp = sprites_grassspringslope_idx;
		if (seasons == 2) temp = sprites_grasssummerslope_idx;
		if (seasons == 3) temp = sprites_grassfallslope_idx;
		if (seasons == 4) temp = sprites_grasswinterslope_idx;
	}
	if (strcmp(boxtype, "water2x1top") == 0){
		if ((seasons == 1 || seasons == 2 || seasons == 3) && watertimer > -1 && watertimer < 150) temp = sprites_waterframe1_idx;
		if ((seasons == 1 || seasons == 2 || seasons == 3) && watertimer > 149 && watertimer < 301) temp = sprites_waterframe2_idx;
		if (seasons == 4) temp = sprites_waterframefrozen_idx;
	}
	if (strcmp(boxtype, "water2x1ground") == 0){
		if (seasons == 1 || seasons == 2 || seasons == 3) temp = sprites_waterground_idx;
		if (seasons == 4) temp = sprites_watergroundfrozen_idx;
	}
	return temp;
}

// Define Collision rules (also see math.c) -----------------------------------

void actboxcoll(int result){
	if (result == 1) grounded = true, Vmomentum = 0;
	if (result == 2) posX = posX + 0.65f, Xmomentum = 0;
	if (result == 3) Vmomentum = 0;
	if (result == 4) posX = posX - 0.65f, Xmomentum = 0;
	if (result == 5) posY = posY - 5;
	if (result == 6) { grounded = true; Vmomentum = 0; posY = posY - 1; }
	if (result == 7) { grounded = true; Vmomentum = 0; posY = posY - 5; }
}

// Put stuff in variables for future rendering -------------------------------

void renderbox(size_t images, int locX, int locY, int scale_X, int scale_Y, int rotationR, int slot){
if ((locX - offsetX > -100) && (locX - offsetX < 500) && didvaluechange[slot] == 0){
spriteimg[slot] = images;
spriteX[slot] = locX;
spriteY[slot] = locY;
scaleX[slot] = scale_X;
scaleY[slot] = scale_Y;
rotation[slot] = rotationR;
loaded[slot] = 1;
didvaluechange[slot] = 1;
} 
else if(didvaluechange[slot] == 1) return;
else loaded[slot] = 0;
}

// Ladder code ----------------------------------------------------

void ladder(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY){
if (IsInsideBox(topleftX, topleftY, boxhorzlength, boxvertlengh, LocX, LocY) != 0){
	if (IsInsideBox(topleftX, topleftY - 38, boxhorzlength, 7, LocX, LocY) != 0) grounded = true;
	else Vmomentum = 0;
	if (keysmove() == 1) { offsetY = offsetY - 2; }
	if (keysmove() == 5 && offsetY < -1) { offsetY = offsetY + 2; }
	else if (keysmove() == 5 && offsetY > -1) { posY = posY + 1; }
}
}

void startL1(){
	// Start code ---------------------------------------------------------------------

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	startSong("romfs:/arcade_thequest.mp3");
	u32 backgroundColor = C2D_Color32f(0,0,0,1);
	C2D_SpriteSheet spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	C2D_SpriteSheet pause = C2D_SpriteSheetLoad("romfs:/gfx/pause.t3x");
	C2D_SpriteSheet text = C2D_SpriteSheetLoad("romfs:/gfx/text.t3x");
    begin:
    consoleInit(GFX_BOTTOM, NULL);
    offsetX = 0, offsetY = 0, i = 0, season = 2, debugdelay = 0, pauseselection = 1;
    posX = 200, posY = 130, Vmomentum = 0, Hmomentum = 0, Xmomentum  = 0, yeettimer = 30, cameratriggeroffset1 = 0;
    pausegame = false, death = false, quit = false, yeet = false, cameratrigger1 = false;


    while (test == false){
    hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();

	if (kDown & KEY_START) pausegame = true;

	//Vertical movement code + offsetcode ------------------------------------------------------------
   	
    if (posX > 270 && Xmomentum > 0) { posX = posX - Xmomentum; offsetX = offsetX + Xmomentum;}
    if (posX < 80 && offsetX > 0 && Xmomentum < 0) { posX = posX - Xmomentum; offsetX = offsetX + Xmomentum;}

    if (offsetY > 0) offsetY = 0;

    if (posY < 50 && Vmomentum < 0 ) { posY = posY - Vmomentum; offsetY = offsetY + Vmomentum;}
    else if (posY < 50 && Vmomentum == 0) { posY = posY + 1; offsetY = offsetY - 1;}
    if (posY > 160 && offsetY < 0 && Vmomentum > 0) { posY = posY - Vmomentum; offsetY = offsetY + Vmomentum;}
	else if (posY > 160 && offsetY < 0 && Vmomentum == 0) { posY = posY - 1; offsetY = offsetY + 1;}

    if (grounded == true && kDown & KEY_A && season == 4) Vmomentum = Vmomentum - 3.0f;
    else if (grounded == true && kDown & KEY_A && season == 1) Vmomentum = Vmomentum - 5.0f;
    else if (grounded == true && kDown & KEY_A) Vmomentum = Vmomentum - 4.0f;
    else if ((kHeld & KEY_A || keysmove() == 1 || keysmove() == 2 || keysmove() == 8) && season == 3 && Vmomentum > 0)  Vmomentum = Vmomentum - 0.12f;

    if (kDown & KEY_L) season = season - 1;
    if (kDown & KEY_R) season = season + 1;
    if (season == 0) season = 4;
    if (season == 5) season = 1;

    if (kDown & KEY_Y) {
    	if (music) music = false;
    		else music = true;
    }

    //Fill the screen with useless debug info -----------------------------------------------

    debugdelay = debugdelay + 1;
    if (debugdelay == 100 && debug == true) {
    	if (flipdebug == false){
	printf("\x1b[8;1HDebug:Starting sprite location print -20");
	debugdelay = 0;
	for(i = 0; i < 21; i++){ printf("\nDebug: Sprite %d: %d %d %d %d %d      ", i, spriteX[i], spriteY[i], scaleX[i], scaleY[i], loaded[i]);
	flipdebug = true; } }
		else if (flipdebug == true){
	printf("\x1b[8;1HDebug:Starting sprite location print -40");
	debugdelay = 0;
	for(i = 20; i < 40; i++){ printf("\nDebug: Sprite %d: %d %d %d %d %d      ", i, spriteX[i], spriteY[i], scaleX[i], scaleY[i], loaded[i]);
	flipdebug = false; } } }
	
	watertimer = watertimer + 7;
	if (debug == true) printf("\x1b[5;1HPos: %f %f  \nOffset: %f %f  \nMomentum: %f %f  ", posX, posY, offsetX, offsetY, Xmomentum, Vmomentum);
	if (watertimer > 300) watertimer = 0;

    if (music == true) RunThread();

    if (season != 4){

    //Part of horizontal movement system ----------------------------

    if (keysmove() == 3 || keysmove() == 2 || keysmove() == 4) Xmomentum = Xmomentum + 0.6f, LRpress = true;
    if (keysmove() == 7 || keysmove() == 6 || keysmove() == 8) Xmomentum = Xmomentum - 0.6f, LRpress = true;

    if (LRpress == true && Xmomentum > 2.0f && season == 1) Xmomentum = Xmomentum - 0.65f;
    else if (LRpress == true && Xmomentum < -2.0f && season == 1) Xmomentum = Xmomentum + 0.65f;
    else if (LRpress == true && Xmomentum > 2.8f && season == 2) Xmomentum = Xmomentum - 0.65f;
    else if (LRpress == true && Xmomentum < -2.8f && season == 2) Xmomentum = Xmomentum + 0.65f;
    else if (LRpress == true && Xmomentum > 2.4f && season == 3) Xmomentum = Xmomentum - 0.65f;
    else if (LRpress == true && Xmomentum < -2.4f && season == 3) Xmomentum = Xmomentum + 0.65f;
    else if (Xmomentum > 0.7f && LRpress == false) Xmomentum = Xmomentum - 0.5f;
    else if (Xmomentum < -0.7f && LRpress == false) Xmomentum = Xmomentum + 0.5f;
    else if (Xmomentum > -0.7f && Xmomentum < 0.7f && LRpress == false) Xmomentum = 0; }

    if (season == 4){

    if (keysmove() == 3 || keysmove() == 2 || keysmove() == 4) Xmomentum = Xmomentum + 0.3f, LRpress = true;
    if (keysmove() == 7 || keysmove() == 6 || keysmove() == 8) Xmomentum = Xmomentum - 0.3f, LRpress = true;

    if (LRpress == true && Xmomentum > 3.75f && grounded == true) Xmomentum = Xmomentum - 0.35f;
    else if (LRpress == true && Xmomentum > 3.0f && grounded == false) Xmomentum = Xmomentum - 0.35f;
    else if (LRpress == true && Xmomentum < -3.75f && grounded == true) Xmomentum = Xmomentum + 0.35f;
    else if (LRpress == true && Xmomentum < -3.0f && grounded == false) Xmomentum = Xmomentum + 0.35f;
    else if (Xmomentum > 0.7f && LRpress == false) Xmomentum = Xmomentum - 0.5f;
    else if (Xmomentum < -0.7f && LRpress == false) Xmomentum = Xmomentum + 0.5f;
    else if (Xmomentum > -0.7f && Xmomentum < 0.7f && LRpress == false) Xmomentum = 0; }

    posX = posX + Xmomentum;
    LRpress = false;

    if (grounded == false) Vmomentum = Vmomentum + 0.15f;
    if (grounded == true && Vmomentum > 0) Vmomentum = 0;
	grounded = false;

	ladder(1015 - offsetX, 40 - offsetY, 20, 150, posX, posY);

	posY = posY + Vmomentum;

	if (posX < 0) posX = 0;

	//Set collision --------------------------------------------------

	actboxcoll(boxcoll(0 - offsetX, 190 - offsetY, 650, 50, posX, posY));
	actboxcoll(boxcoll(700 - offsetX, 190 - offsetY, 850, 50, posX, posY));
	actboxcoll(boxcoll(1050 - offsetX, 65 - offsetY, 200, 400, posX, posY));
	if (season == 4) actboxcoll(boxcoll(1550 - offsetX, 197 - offsetY, 300, 50, posX, posY));
	actboxcoll(boxcoll(1850 - offsetX, 190 - offsetY, 300, 50, posX, posY));
	actboxcoll(boxcoll(2150 - offsetX, 115 - offsetY, 200, 300, posX, posY));
	actboxcoll(boxcoll(2550 - offsetX, 190 - offsetY, 200, 100, posX, posY));
	if (boxcoll(2275 - offsetX, 40 - offsetY, 200, 100, posX, posY) != 0 && cameratriggeroffset1 < 75) {
		cameratrigger1 = true;
	}


	if (cameratriggeroffset1 > 74) cameratrigger1 = false;

	if (cameratrigger1 == true){
		cameratriggeroffset1 = cameratriggeroffset1 + 1;
		offsetX = offsetX + 3;
		posX = posX - 3;
	}

	if (boxcoll(2700 - offsetX, 120 - offsetY, 50, 50, posX, posY) != 0 && yeet == false){ 
		yeet = true;
		for (i = 0; i < 10; i++) { textboxX[i] = 5000; }
		textboxX[7] = 2400;
		textboxX[8] = 2600;
		}


	if (yeet == true){
		actboxcoll(boxcoll(2350 - offsetX, 190 - offsetY, 200, 100, posX, posY));
		actboxcoll(boxcoll(1250 - offsetX, 140 - offsetY, 50, 50, posX, posY));
		if (boxcoll(50 - offsetX, 120 - offsetY, 50, 50, posX, posY) != 0) quit = true;
	}

	//EndRunCode -----------------------------------------------------
	if (yeet == true){
		printf("\x1b[1;1H\x1b[41mRUN!RUN!RUN!RUN!RUN!RUN!RUN!RUN!RUN!RUN!Time remaining: %f  \x1b[40m", yeettimer);
		yeettimer = yeettimer - 0.015f;
		if (yeettimer < 0) death = true;
	}

	//Print music n shit ----------------------------------------------
	if (music == true){
	printf("\x1b[3;1H---- Music Playing ----\nThe Quest");
	}
	printf("\x1b[5;1HPlaying Level: Tutorial");

	//Define Sprites -------------------------------------------------
	//size_t images, int locX, int locY, int scale_X, int scale_Y, int rotationR, int slot

	renderbox(getspriteseason("grass2x1", season), 0, 190, 1, 1, 0, 1);
	renderbox(getspriteseason("grass2x1", season), 100, 190, 1, 1, 0, 3);
	renderbox(getspriteseason("grass2x1", season), 200, 190, 1, 1, 0, 5);
	renderbox(getspriteseason("grass2x1", season), 300, 190, 1, 1, 0, 7);
	renderbox(getspriteseason("grass2x1", season), 400, 190, 1, 1, 0, 9);
	renderbox(getspriteseason("grass2x1", season), 500, 190, 1, 1, 0, 11);
	renderbox(getspriteseason("ground2x1", season), 0, 215, 1, 1, 0, 2);
	renderbox(getspriteseason("ground2x1", season), 100, 215, 1, 1, 0, 4);
	renderbox(getspriteseason("ground2x1", season), 200, 215, 1, 1, 0, 6);
	renderbox(getspriteseason("ground2x1", season), 300, 215, 1, 1, 0, 8);
	renderbox(getspriteseason("ground2x1", season), 400, 215, 1, 1, 0, 10);
	renderbox(getspriteseason("ground2x1", season), 500, 215, 1, 1, 0, 12);
	renderbox(getspriteseason("grassslope", season), 600, 190, -1, 1, 0, 13);
	renderbox(getspriteseason("ground", season), 600, 215, 1, 1, 0, 14);

	if (yeet == true) renderbox(sprites_endlevel_idx, 50, 120, 1, 1, 0, 35);

	renderbox(getspriteseason("grassslope", season), 700, 190, 1, 1, 0, 15);
	renderbox(getspriteseason("ground", season), 700, 215, 1, 1, 0, 16);
	renderbox(getspriteseason("grass2x1", season), 750, 190, 1, 1, 0, 17);
	renderbox(getspriteseason("ground2x1", season), 750, 215, 1, 1, 0, 18);
	renderbox(getspriteseason("grass2x1", season), 850, 190, 1, 1, 0, 20);
	renderbox(getspriteseason("ground2x1", season), 850, 215, 1, 1, 0, 21);
	renderbox(getspriteseason("grass2x1", season), 950, 190, 1, 1, 0, 22);
	renderbox(getspriteseason("ground2x1", season), 950, 215, 1, 1, 0, 23);

	renderbox(getspriteseason("ground2x1", season), 1050, 215, 1, 1, 0, 24);
	renderbox(getspriteseason("ground2x1", season), 1050, 190, 1, 1, 0, 25);
	renderbox(getspriteseason("ground2x1", season), 1050, 165, 1, 1, 0, 26);
	renderbox(getspriteseason("ground2x1", season), 1050, 140, 1, 1, 0, 27);
	renderbox(getspriteseason("ground2x1", season), 1050, 115, 1, 1, 0, 28);
	renderbox(getspriteseason("ground2x1", season), 1050, 90, 1, 1, 0, 29);
    renderbox(getspriteseason("grassslope", season), 1050, 65, 1, 1, 0, 30);
    renderbox(getspriteseason("grass2x1", season), 1100, 65, 1, 1, 0, 31);
    renderbox(getspriteseason("grassslope", season), 1200, 65, -1, 1, 0, 32);
    renderbox(getspriteseason("ground2x1", season), 1150, 215, 1, 1, 0, 33);
	renderbox(getspriteseason("ground2x1", season), 1150, 190, 1, 1, 0, 34);
	renderbox(getspriteseason("ground2x1", season), 1150, 165, 1, 1, 0, 35);
	renderbox(getspriteseason("ground2x1", season), 1150, 140, 1, 1, 0, 36);
	renderbox(getspriteseason("ground2x1", season), 1150, 115, 1, 1, 0, 37);
	renderbox(getspriteseason("ground2x1", season), 1150, 90, 1, 1, 0, 38);

	renderbox(getspriteseason("devbox", season), 1250, 165, 1, 1, 0, 0);
	renderbox(getspriteseason("devbox", season), 1250, 140, 1, 1, 0, 20);

	renderbox(sprites_ladder_idx, 1015, 140, 1, 1, 0, 39);
	renderbox(sprites_ladder_idx, 1015, 90, 1, 1, 0, 1);
	renderbox(sprites_ladder_idx, 1015, 40, 1, 1, 0, 2);

	renderbox(getspriteseason("grass2x1", season), 1250, 190, 1, 1, 0, 3);
	renderbox(getspriteseason("ground2x1", season), 1250, 215, 1, 1, 0, 4);
	renderbox(getspriteseason("grass2x1", season), 1350, 190, 1, 1, 0, 5);
	renderbox(getspriteseason("ground2x1", season), 1350, 215, 1, 1, 0, 6);
	renderbox(getspriteseason("grass2x1", season), 1450, 190, 1, 1, 0, 7);
	renderbox(getspriteseason("ground2x1", season), 1450, 215, 1, 1, 0, 8);

	renderbox(getspriteseason("water2x1top", season), 1550, 190, 1, 1, 0, 9);
	renderbox(getspriteseason("water2x1ground", season), 1550, 215, 1, 1, 0, 10);
	renderbox(getspriteseason("water2x1top", season), 1650, 190, 1, 1, 0, 11);
	renderbox(getspriteseason("water2x1ground", season), 1650, 215, 1, 1, 0, 12);
	renderbox(getspriteseason("water2x1top", season), 1750, 190, 1, 1, 0, 13);
	renderbox(getspriteseason("water2x1ground", season), 1750, 215, 1, 1, 0, 14);

	renderbox(getspriteseason("grass2x1", season), 1850, 190, 1, 1, 0, 15);
	renderbox(getspriteseason("ground2x1", season), 1850, 215, 1, 1, 0, 16);
	renderbox(getspriteseason("grass2x1", season), 1950, 190, 1, 1, 0, 17);
	renderbox(getspriteseason("ground2x1", season), 1950, 215, 1, 1, 0, 18);
	renderbox(getspriteseason("grass2x1", season), 2050, 190, 1, 1, 0, 19);
	renderbox(getspriteseason("ground2x1", season), 2050, 215, 1, 1, 0, 20);

	renderbox(getspriteseason("ground2x1", season), 2150, 215, 1, 1, 0, 21);
	renderbox(getspriteseason("ground2x1", season), 2150, 190, 1, 1, 0, 22);
	renderbox(getspriteseason("ground2x1", season), 2150, 165, 1, 1, 0, 23);
	renderbox(getspriteseason("ground2x1", season), 2150, 140, 1, 1, 0, 24);
	renderbox(getspriteseason("ground2x1", season), 2250, 215, 1, 1, 0, 25);
	renderbox(getspriteseason("ground2x1", season), 2250, 190, 1, 1, 0, 26);
	renderbox(getspriteseason("ground2x1", season), 2250, 165, 1, 1, 0, 27);
	renderbox(getspriteseason("ground2x1", season), 2250, 140, 1, 1, 0, 28);
	renderbox(getspriteseason("grassslope", season), 2150, 115, 1, 1, 0, 29);
	renderbox(getspriteseason("grass2x1", season), 2200, 115, 1, 1, 0, 30);
	renderbox(getspriteseason("grassslope", season), 2300, 115, -1, 1, 0, 31);
	
	renderbox(getspriteseason("devbox", season), 2350, 190, 1, 1, 0, 32);
	renderbox(getspriteseason("devbox", season), 2350, 215, 1, 1, 0, 33);
	renderbox(getspriteseason("devbox", season), 2400, 190, 1, 1, 0, 34);
	renderbox(getspriteseason("devbox", season), 2400, 215, 1, 1, 0, 35);
	renderbox(getspriteseason("devbox", season), 2450, 190, 1, 1, 0, 36);
	renderbox(getspriteseason("devbox", season), 2450, 215, 1, 1, 0, 37);
	renderbox(getspriteseason("devbox", season), 2500, 190, 1, 1, 0, 38);
	renderbox(getspriteseason("devbox", season), 2500, 215, 1, 1, 0, 39);

	renderbox(getspriteseason("grassslope", season), 2550, 190, 1, 1, 0, 1);
	renderbox(getspriteseason("grass2x1", season), 2600, 190, 1, 1, 0, 2);
	renderbox(getspriteseason("grassslope", season), 2700, 190, -1, 1, 0, 3);
	renderbox(getspriteseason("ground2x1", season), 2550, 215, 1, 1, 0, 4);
	renderbox(getspriteseason("ground2x1", season), 2650, 215, 1, 1, 0, 5);

	if (yeet == false) renderbox(sprites_orb_idx, 2700, 120, 1, 1, 0, 6);



	//reminder to code in rushexit, and define summer and fall


	//Render frame -------------------------------------------

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_SceneBegin(top);
    C2D_TargetClear(top, backgroundColor);
    if (offsetX / 3 < 590) C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 0 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
    if (offsetX / 3 > 189 && offsetX / 3 < 1178) C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 589 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
    if (offsetX / 3 > 689) C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 1178 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_devblock_idx), posX, posY, 0.5f, NULL, 1, 2);

    tempvert = offsetY;
    temphorz = offsetX;


    for (i = 0; i < 41; i++){
    if (loaded[i] == 1) {
    	if (rotation[i] != 0) C2D_DrawImageAtRotated(C2D_SpriteSheetGetImage(spritesheet, spriteimg[i]), spriteX[i] - offsetX, spriteY[i] - offsetY, 0.5f, rotation[i], NULL, scaleX[i], scaleY[i]);	
    	else C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, spriteimg[i]), spriteX[i] - temphorz, spriteY[i] - tempvert, 0.5f, NULL, scaleX[i], scaleY[i]); }	
    }

    for (i = 0; i < 9; i++){
    	    //Might need cleaning later
    		if (textboxX[i] - offsetX > -100 && textboxX[i] - offsetX < 500 && textboxX != 0){
    		C2D_DrawImageAt(C2D_SpriteSheetGetImage(text, text_textbox_idx), textboxX[i] - offsetX, textboxY[i] - offsetY, 0.5f, NULL, 1, 1);
			C2D_DrawImageAt(C2D_SpriteSheetGetImage(text, textboxtext[i]), textboxX[i] - offsetX + 5, textboxY[i] - offsetY + 5, 0.5f, NULL, 1, 1); }
    }



	C3D_FrameEnd(0);
	for (i = 0; i < 41; i++){
	if (didvaluechange[i] == 1) didvaluechange[i] = 0; }

	if (posY > 400) death = true;

	//Pause code -------------------------------------------

	while (pausegame == true || death == true){
		circlePosition pos;
		hidCircleRead(&pos);
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_DUP || pos.dy > 80) pauseselection = pauseselection - 1;
		if (kDown & KEY_DDOWN || pos.dy < -80) pauseselection = pauseselection + 1;
		if (pauseselection == 0) pauseselection = 1;
		if (pauseselection == 4) pauseselection = 3;
		if (death == true && pauseselection == 1) pauseselection = 2;
		if (kDown & KEY_A && pauseselection == 1) pausegame = false;
		if (kDown & KEY_A && pauseselection == 2) goto begin;
		if (kDown & KEY_A && pauseselection == 3) { quit = true; break; }
		if (pauseselection == 1) arrowoffset = 64;
		if (pauseselection == 2) arrowoffset = 86;
		if (pauseselection == 3) arrowoffset = 106;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
   	    C2D_SceneBegin(top);
   	    C2D_TargetClear(top, backgroundColor);

   	    if (offsetX / 3 < 590) C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 0 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
    	if (offsetX / 3 > 189 && offsetX / 3 < 1178) C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 589 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
    	if (offsetX / 3 > 689) C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_smwback_idx), 1178 - (offsetX / 3), 0, 0.5f, NULL, 1, 1);
   	    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_devblock_idx), posX, posY, 0.5f, NULL, 1, 2);



   		for (i = 0; i < 41; i++){
   		 if (loaded[i] == 1) {
    	 if (rotation[i] != 0) C2D_DrawImageAtRotated(C2D_SpriteSheetGetImage(spritesheet, spriteimg[i]), spriteX[i] - offsetX, spriteY[i] - offsetY, 0.5f, rotation[i], NULL, scaleX[i], scaleY[i]);	
    	 else C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, spriteimg[i]), spriteX[i] - offsetX, spriteY[i] - offsetY, 0.5f, NULL, scaleX[i], scaleY[i]); }	
   		 }

    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(pause, pause_fade_idx), 0, 0, 0.5f, NULL, 1, 1);

    	if (death == true) C2D_DrawImageAt(C2D_SpriteSheetGetImage(pause, pause_youdied_idx), 20, 20, 0.5f, NULL, 1, 1);
    	if (pausegame == true) C2D_DrawImageAt(C2D_SpriteSheetGetImage(pause, pause_pausemenu_idx), 20, 20, 0.5f, NULL, 1, 1);
    	if (pausegame == true) C2D_DrawImageAt(C2D_SpriteSheetGetImage(pause, pause_pausecontinue_idx), 20, 60, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(pause, pause_pausemain_idx), 20, 80, 0.5f, NULL, 1, 1);
    	C2D_DrawImageAt(C2D_SpriteSheetGetImage(pause, pause_arrow_idx), 100, arrowoffset, 0.5f, NULL, 1, 1);
	    C3D_FrameEnd(0);
	}

	//End ----------------------------------------------

	if (quit) break;

	 }
	 C2D_SpriteSheetFree(spritesheet);
	 C2D_SpriteSheetFree(pause);
	 C2D_SpriteSheetFree(text);
	 exitSong();
	 C2D_Fini();
	 C3D_Fini();
	 gfxExit();
}