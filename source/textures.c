#include "textures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <unistd.h>
#include "text.h"
#include "pause.h"
#include "sprites.h"
#include "background.h"
#include "mainmenu.h"

static C2D_SpriteSheet spritesheet;
static C2D_SpriteSheet background;
static C2D_SpriteSheet mainmenu;
static C2D_SpriteSheet pausemenu;

C2D_TextBuf g_dynamicBuf;

void texload(){
	spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	//text = C2D_SpriteSheetLoad("romfs:/gfx/text.t3x");
	background = C2D_SpriteSheetLoad("romfs:/gfx/background.t3x");
	mainmenu = C2D_SpriteSheetLoad("romfs:/gfx/mainmenu.t3x");
	pausemenu = C2D_SpriteSheetLoad("romfs:/gfx/pause.t3x");

	player = C2D_SpriteSheetGetImage(spritesheet, sprites_devblock_idx);
	escbl1 = C2D_SpriteSheetGetImage(spritesheet, sprites_devbox_idx);
	escbl2 = C2D_SpriteSheetGetImage(spritesheet, sprites_devbox2_idx);
	escbl3 = C2D_SpriteSheetGetImage(spritesheet, sprites_devbox3_idx);
	escbl4 = C2D_SpriteSheetGetImage(spritesheet, sprites_devbox4_idx);
	grfall = C2D_SpriteSheetGetImage(spritesheet, sprites_grassfall_idx);
	grfall2x = C2D_SpriteSheetGetImage(spritesheet, sprites_grassfall2x1_idx);
	grspri = C2D_SpriteSheetGetImage(spritesheet, sprites_grassspring_idx);
	grspri2x = C2D_SpriteSheetGetImage(spritesheet, sprites_grassspring2x1_idx);
	grsumm = C2D_SpriteSheetGetImage(spritesheet, sprites_grasssummer_idx);
	grsumm2x = C2D_SpriteSheetGetImage(spritesheet, sprites_grasssummer2x1_idx);
	grwint = C2D_SpriteSheetGetImage(spritesheet, sprites_grasswinter_idx);
	grwint2x = C2D_SpriteSheetGetImage(spritesheet, sprites_grasswinter2x1_idx);
	gnfall = C2D_SpriteSheetGetImage(spritesheet, sprites_groundfall_idx);
	gnfall2x = C2D_SpriteSheetGetImage(spritesheet, sprites_groundfall2x1_idx);
	gnspri = C2D_SpriteSheetGetImage(spritesheet, sprites_groundspring_idx);
	gnspri2x = C2D_SpriteSheetGetImage(spritesheet, sprites_groundspring2x1_idx);
	gnsumm = C2D_SpriteSheetGetImage(spritesheet, sprites_groundsummer_idx);
	gnsumm2x = C2D_SpriteSheetGetImage(spritesheet, sprites_groundsummer2x1_idx);
	gnwint = C2D_SpriteSheetGetImage(spritesheet, sprites_groundwinter_idx);
	gnwint2x = C2D_SpriteSheetGetImage(spritesheet, sprites_groundwinter2x1_idx);
	grfallslp = C2D_SpriteSheetGetImage(spritesheet, sprites_grassfallslope_idx);
	grsprislp = C2D_SpriteSheetGetImage(spritesheet, sprites_grassspringslope_idx);
	grsummslp = C2D_SpriteSheetGetImage(spritesheet, sprites_grasssummerslope_idx);
	grwintslp = C2D_SpriteSheetGetImage(spritesheet, sprites_grasswinterslope_idx);
	water1 = C2D_SpriteSheetGetImage(spritesheet, sprites_waterframe1_idx);
	water2 = C2D_SpriteSheetGetImage(spritesheet, sprites_waterframe2_idx);
	watergrnd = C2D_SpriteSheetGetImage(spritesheet, sprites_waterground_idx);
	waterfrz = C2D_SpriteSheetGetImage(spritesheet, sprites_waterframefrozen_idx);
	watergrndfrz = C2D_SpriteSheetGetImage(spritesheet, sprites_watergroundfrozen_idx);
	laddertex = C2D_SpriteSheetGetImage(spritesheet, sprites_ladder_idx);
	escblinv = C2D_SpriteSheetGetImage(spritesheet, sprites_devboxmissing_idx);
	escorb = C2D_SpriteSheetGetImage(spritesheet, sprites_orb_idx);
	escflag = C2D_SpriteSheetGetImage(spritesheet, sprites_endlevel_idx);
	coin = C2D_SpriteSheetGetImage(spritesheet, sprites_coin_idx);

	fadescr = C2D_SpriteSheetGetImage(mainmenu, mainmenu_fade_idx);
	titlemenu = C2D_SpriteSheetGetImage(mainmenu, mainmenu_title_idx);
	menutxt = C2D_SpriteSheetGetImage(mainmenu, mainmenu_main_idx);
	menulvltxt = C2D_SpriteSheetGetImage(mainmenu, mainmenu_levels_idx);
	menuarr = C2D_SpriteSheetGetImage(mainmenu, mainmenu_arrow_idx);
	menuset = C2D_SpriteSheetGetImage(mainmenu, mainmenu_settingimg_idx);
	menuoff = C2D_SpriteSheetGetImage(mainmenu, mainmenu_off_idx);
	menuon = C2D_SpriteSheetGetImage(mainmenu, mainmenu_on_idx);

	pausecont = C2D_SpriteSheetGetImage(pausemenu, pause_pausecontinue_idx);
	pausemain = C2D_SpriteSheetGetImage(pausemenu, pause_pausemain_idx);
	pausetxt = C2D_SpriteSheetGetImage(pausemenu, pause_pausemenu_idx);
	pausedied = C2D_SpriteSheetGetImage(pausemenu, pause_youdied_idx);

	backdev = C2D_SpriteSheetGetImage(background, background_devback_idx);
	backsmw = C2D_SpriteSheetGetImage(background, background_smwback_idx);
	fadetxt = C2D_SpriteSheetGetImage(background, background_fadetext_idx);


}

void texfree() {
	C2D_SpriteSheetFree(spritesheet);
	//C2D_SpriteSheetFree(text);
	C2D_SpriteSheetFree(background);
	C2D_SpriteSheetFree(mainmenu);
	C2D_SpriteSheetFree(pausemenu);
}


int watertimer = 0;

C2D_Image getsprite(int boxtype, int seasons, int run){
	C2D_Image temp = escbl1;
	if (boxtype == 0){
		if (run == 1){
		if (seasons == 1) temp = escbl1;
		if (seasons == 2) temp = escbl2;
		if (seasons == 3) temp = escbl3;
		if (seasons == 4) temp = escbl4; }
		else temp = escblinv;
	}
	if (boxtype == 1){
		if (seasons == 1) temp = grspri;
		if (seasons == 2) temp = grsumm;
		if (seasons == 3) temp = grfall;
		if (seasons == 4) temp = grwint;
	}
	if (boxtype == 2){
		if (seasons == 1) temp = gnspri;
		if (seasons == 2) temp = gnsumm;
		if (seasons == 3) temp = gnfall;
		if (seasons == 4) temp = gnwint;
	}
	if (boxtype == 3){
		if (seasons == 1) temp = grspri2x;
		if (seasons == 2) temp = grsumm2x;
		if (seasons == 3) temp = grfall2x;
		if (seasons == 4) temp = grwint2x;
	}
	if (boxtype == 4){
		if (seasons == 1) temp = gnspri2x;
		if (seasons == 2) temp = gnsumm2x;
		if (seasons == 3) temp = gnfall2x;
		if (seasons == 4) temp = gnwint2x;
	}
	if (boxtype == 5){
		if (seasons == 1) temp = grsprislp;
		if (seasons == 2) temp = grsummslp;
		if (seasons == 3) temp = grfallslp;
		if (seasons == 4) temp = grwintslp;
	}
	if (boxtype == 6){
		watertimer++;
		//bug, need to check if this is first run per frame!
		if (watertimer > 50) watertimer = 0;
		if ((seasons == 1 || seasons == 2 || seasons == 3) && watertimer > -1 && watertimer < 25) temp = water1;
		if ((seasons == 1 || seasons == 2 || seasons == 3) && watertimer > 24 && watertimer < 51) temp = water2;
		if (seasons == 4) temp = waterfrz;
	}
	if (boxtype == 7){
		if (seasons == 1 || seasons == 2 || seasons == 3) temp = watergrnd;
		if (seasons == 4) temp = watergrndfrz;
	}
	if (boxtype == 8) temp = laddertex;
	if (boxtype == 9) temp = coin;
	if (boxtype == 10) temp = fadetxt;

	return temp;
}

C2D_Image backgroundimg(int backtype){
	C2D_Image temp = backdev;
	if (backtype == 1) temp = backsmw;
	return temp;
}

char* musicget(int musictype){
	char* temp = "";
	if (musictype == 1) temp = "romfs:/arcade_thequest.mp3";
	return temp;
}

void Init_Text(){
	g_dynamicBuf = C2D_TextBufNew(4096);
}

void Draw_Text(int x, int y, float size, const char *text){
	C2D_Text textbuff;
	C2D_TextParse(&textbuff, g_dynamicBuf, text);
	C2D_TextOptimize(&textbuff);
	C2D_DrawText(&textbuff, C2D_WithColor, x, y, 0.5f, size, size, C2D_Color32f(1,1,1,1));
	C2D_TextBufClear(g_dynamicBuf);
}