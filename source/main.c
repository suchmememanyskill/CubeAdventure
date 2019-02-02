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

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

	while (aptMainLoop())
	{
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_SELECT) break; 
		printf("\x1b[1;1HPress A to start L1, start to exit");
		if (kDown & KEY_A) {
		consoleInit(GFX_BOTTOM, NULL);
		startL1(); }
		gspWaitForVBlank();

	}
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();

	gfxExit();
	return 0;
}