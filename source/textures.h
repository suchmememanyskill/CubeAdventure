#pragma once
#include <stddef.h>
#include <citro2d.h>

C2D_Image player, escbl1, escbl2, escbl3, escbl4, grfall, grfall2x, grspri, grspri2x, grsumm, grsumm2x, grwint, grwint2x, gnfall, gnfall2x, gnspri, gnspri2x, gnsumm, gnsumm2x, gnwint, gnwint2x, grfallslp, grsprislp, grsummslp, grwintslp, water1, water2, watergrnd, waterfrz, watergrndfrz, laddertex, escblinv, escorb, escflag, fadescr, titlemenu, menutxt, menulvltxt, menuarr, menuset, menuoff, menuon, pausecont, pausemain, pausetxt, pausedied, backdev, backsmw, coin, fadetxt;

void texload(void);
void texfree(void);
C2D_Image getsprite(int boxtype, int seasons, int run);
C2D_Image backgroundimg(int backtype);
char* musicget(int musictype);
void Init_Text(void);
void Draw_Text(int x, int y, float size, const char *text);